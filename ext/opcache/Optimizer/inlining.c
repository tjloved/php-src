/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Nikita Popov <nikic@php.net>                                |
   +----------------------------------------------------------------------+
*/

#include "php.h"
#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/statistics.h"

/* This pass implements function inling. Currently only inlining of free functions that meet certain
 * relatively stringent requirements is possible.
 *
 * For a function func($a, $b) with opcodes
 *
 * $a = RECV 1
 * $b = RECV 2
 * T0 = ADD $a, $b
 * ASSIGN $c, T0
 * RETURN $c
 *
 * a call
 *
 * INIT_FCALL "func"
 * SEND_VAL 42
 * SEND_VAR $var
 * V1 = DO_UCALL
 * L:
 * ...
 *
 * will be transformed into
 *
 * ASSIGN $a, 42
 * ASSIGN $b, $var
 * T0 = ADD $a, $b
 * ASSIGN $c, T0
 * V1 = QM_ASSIGN $c
 * JMP L
 * L:
 * UNSET_VAR $a
 * UNSET_VAR $b
 * UNSET_VAR $c
 * ...
 *
 * The UNSETs are inserted to make sure that a) variables do not live longer due to inlining and
 * b) references are broken. The assumption / hope is that DCE will elimiate these UNSETs again and
 * SCCP and copy propagation will eliminate the ASSIGNs.
 *
 * Note that inlining may result in there existing multiple CVs with the same name.
 *
 * There is no proper inlining heuristic yet -- we pretty much inline everything we can that isn't
 * excessively large. Inlining happens in multiple passes, the current choice of 3 passes is
 * motivated by SSA construction overflowing the stack if you try more...
 */

typedef struct _merge_info {
	uint32_t tmp_offset;
	/* Shiftlist for instructions */
	int32_t *shiftlist;
	/* Map from old live_range offsets to new ones */
	int32_t *live_range_map;
} merge_info;

typedef struct _inline_info {
	struct _inline_info *next;
	zend_op_array *fbc;
	zend_op *init_opline;
	zend_op *call_opline;
	int32_t opnum_diff;
	uint32_t num_inlined_opcodes;
	uint32_t num_args_passed;
	uint32_t result_var_num;
	uint8_t result_var_type;
	zend_bool is_this_call;
	merge_info merge;
} inline_info;

static zend_bool can_inline_opcodes(
		zend_op_array *op_array, zend_bool rt_constants, zend_bool from) {
	zend_op *opline = op_array->opcodes, *end = opline + op_array->last;
	for (; opline != end; opline++) {
		if (from) {
			uint32_t flags = zend_get_opcode_flags(opline->opcode);
			uint32_t op1_flags = ZEND_VM_OP1_FLAGS(flags) & ZEND_VM_OP_MASK;
			uint32_t op2_flags = ZEND_VM_OP2_FLAGS(flags) & ZEND_VM_OP_MASK;
			uint32_t ext_flags = flags & ZEND_VM_EXT_MASK;

			/* Check if LSB is used. It would be nice to just replace the correct class name in the
			 * future, however right now adding the extra literals / cache slots on the fly would be
			 * way too much hassle. */
			if (op1_flags == ZEND_VM_OP_CLASS_FETCH && opline->op1_type == IS_UNUSED
					&& (opline->op1.num & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_STATIC) {
				return 0;
			}
			if (op2_flags == ZEND_VM_OP_CLASS_FETCH && opline->op2_type == IS_UNUSED
					&& (opline->op2.num & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_STATIC) {
				return 0;
			}
			if (ext_flags == ZEND_VM_EXT_CLASS_FETCH &&
					(opline->extended_value & ZEND_FETCH_CLASS_MASK) == ZEND_FETCH_CLASS_STATIC) {
				return 0;
			}
		}

		switch (opline->opcode) {
			case ZEND_RECV_INIT:
				if (from) {
					zval *zv = CRT_CONSTANT_EX(op_array, opline->op2, rt_constants);
					if (Z_CONSTANT_P(zv)) {
						/* If initializer is constant or AST we cannot convert it into
						 * a simple assignment (as these don't update constants) */
						return 0;
					}
				}
				break;
			case ZEND_INIT_FCALL:
			case ZEND_INIT_NS_FCALL_BY_NAME:
			{
				zval *zv = CRT_CONSTANT_EX(op_array, opline->op2, rt_constants);
				zend_function *fn;
				if (opline->opcode == ZEND_INIT_NS_FCALL_BY_NAME) {
					/* The third literal is the lowercased unqualified name */
					zv += 2;
				}
				if ((fn = zend_hash_find_ptr(EG(function_table), Z_STR_P(zv))) != NULL) {
					if (fn->type == ZEND_INTERNAL_FUNCTION) {
						if (zend_string_equals_literal(Z_STR_P(zv), "extract") ||
							zend_string_equals_literal(Z_STR_P(zv), "compact") ||
							zend_string_equals_literal(Z_STR_P(zv), "parse_str") ||
							zend_string_equals_literal(Z_STR_P(zv), "mb_parse_str") ||
							zend_string_equals_literal(Z_STR_P(zv), "get_defined_vars")
						) {
							/* Functions observing the symbol table */
							return 0;
						} else if (zend_string_equals_literal(Z_STR_P(zv), "debug_backtrace") ||
								   zend_string_equals_literal(Z_STR_P(zv), "debug_print_backtrace")) {
							return 0;
						}
						if (from) {
							if (zend_string_equals_literal(Z_STR_P(zv), "func_num_args") ||
								zend_string_equals_literal(Z_STR_P(zv), "func_get_arg") ||
								zend_string_equals_literal(Z_STR_P(zv), "func_get_args")
							) {
								/* Could theoretically be supported, but not worth patching
								 * them up */
								return 0;
							}
							if (zend_string_equals_literal(Z_STR_P(zv), "get_called_class") ||
								zend_string_equals_literal(Z_STR_P(zv), "forward_static_call") ||
								zend_string_equals_literal(Z_STR_P(zv), "forward_static_call_array")
							) {
								/* Functions observing LSB scope */
								return 0;
							}
						}
					}
				}
				break;
			}
			case ZEND_THROW:
				/* If inlining is in use, there can be no expectation of keeping consistent
				 * backtraces. However, there's no need to be too blatant with it. */
				return 0;
			/* Various cases of indirect variable access */
			case ZEND_INCLUDE_OR_EVAL:
				return 0;
			case ZEND_UNSET_VAR:
			case ZEND_ISSET_ISEMPTY_VAR:
				if ((opline->extended_value & ZEND_FETCH_TYPE_MASK) == ZEND_FETCH_LOCAL
						&& !(opline->extended_value & ZEND_QUICK_SET)) {
					return 0;
				}
				break;
			case ZEND_FETCH_R:
			case ZEND_FETCH_W:
			case ZEND_FETCH_RW:
			case ZEND_FETCH_FUNC_ARG:
			case ZEND_FETCH_IS:
			case ZEND_FETCH_UNSET:
				if ((opline->extended_value & ZEND_FETCH_TYPE_MASK) == ZEND_FETCH_LOCAL) {
					return 0;
				}
				break;
		}
	}
	return 1;
}

static inline zend_bool can_inline_into(zend_op_array *op_array) {
	return can_inline_opcodes(op_array, 0, 0);
}

/* Inlining heuristic */
static inline zend_bool should_inline(
		zend_op_array *target, zend_op_array *source, uint32_t num_const_args, uint32_t pass) {
	if (source->last_try_catch) {
		/* The DFA optimizations don't support op_arrays with try/catch, so don't inline them. */
		return 0;
	}
	if (source == target && pass > 1) {
		/* Inline recursive functions to two levels only */
		return 0;
	}
	if (source->last > 500) {
		return 0;
	}
	if (source->last_var > 1 + num_const_args) {
		return 0;
	}
	if (source->last > 30 + 10 * num_const_args) {
		return 0;
	}
	return 1;
}

static zend_bool can_inline_from(
		zend_op_array *op_array, uint32_t num_args_passed, zend_bool rt_constants) {
	int i;
	uint32_t forbidden_flags = ZEND_ACC_GENERATOR | ZEND_ACC_VARIADIC | ZEND_ACC_RETURN_REFERENCE
	                         | ZEND_ACC_HAS_TYPE_HINTS | ZEND_ACC_HAS_RETURN_TYPE
							 | ZEND_ACC_HAS_FINALLY_BLOCK;

	// TODO Collect variadics into array
	// TODO Allow arguments where constant is passed to type-hinted argument
	// TODO Support by-ref return (?)
	if (op_array->fn_flags & forbidden_flags) {
		return 0;
	}
	if (op_array->early_binding != (uint32_t) -1) {
		return 0;
	}
	// TODO Support merging static variables
	if (op_array->static_variables) {
		return 0;
	}
	// TODO Support discarding too many arguments
	if (num_args_passed < op_array->required_num_args
			|| num_args_passed > op_array->num_args) {
		return 0;
	}
	// TODO Support pass by reference
	for (i = 0; i < op_array->num_args; ++i) {
		zend_arg_info *arg_info = &op_array->arg_info[i];
		if (arg_info->pass_by_reference) {
			return 0;
		}
	}
	return can_inline_opcodes(op_array, rt_constants, 1);
}

static zend_bool can_inline_scope(zend_op_array *from, zend_op_array *into) {
	if (!from->scope) {
		/* Non-methods can always be inlined */
		return 1;
	}

	if (into->scope != from->scope) {
		/* Different scopes, can't inline for now */
		return 0;
	}

	if (into->scope->ce_flags & ZEND_ACC_TRAIT) {
		/* Scope in traits is not known */
		return 0;
	}

	return 1;
}

static uint32_t num_returns(zend_op_array *op_array) {
	uint32_t i, num = 0;
	for (i = 0; i < op_array->last; i++) {
		num += op_array->opcodes[i].opcode == ZEND_RETURN;
	}
	return num;
}

static int32_t opnum_diff(
		const zend_op_array *op_array, uint32_t num_inlined_opcodes, zend_bool is_this_call) {
	int32_t diff = num_inlined_opcodes;

	/* We add a $this != NULL check */
	if (is_this_call) {
		diff += 1;
	}

	/* We drop the INIT_FCALL and DO_FCALL opcodes */
	diff -= 2;
	/* We will add UNSETs for every CV of the inlined function */
	diff += op_array->last_var;

	return diff;
}

static inline zend_bool is_init_opline(zend_op *opline) {
	switch (opline->opcode) {
		case ZEND_INIT_FCALL_BY_NAME:
		case ZEND_INIT_NS_FCALL_BY_NAME:
		case ZEND_NEW:
		case ZEND_INIT_DYNAMIC_CALL:
		case ZEND_INIT_METHOD_CALL:
		case ZEND_INIT_STATIC_METHOD_CALL:
		case ZEND_INIT_FCALL:
		case ZEND_INIT_USER_CALL:
			return 1;
		default:
			return 0;
	}
}

static inline zend_bool is_call_opline(zend_op *opline) {
	switch (opline->opcode) {
		case ZEND_DO_FCALL:
		case ZEND_DO_ICALL:
		case ZEND_DO_UCALL:
		case ZEND_DO_FCALL_BY_NAME:
			return 1;
		default:
			return 0;
	}
}

/* Returns NULL if unsupported call type */
static zend_op *find_call_opline(zend_op *opline) {
	unsigned level = 0;
	for (;;) {
		opline++;
		if (is_init_opline(opline)) {
			level++;
		} else if (is_call_opline(opline)) {
			if (level == 0) {
				return opline;
			}
			level--;
		} else if (opline->opcode == ZEND_SEND_UNPACK
				|| opline->opcode == ZEND_SEND_ARRAY
				|| opline->opcode == ZEND_SEND_VAR_NO_REF) {
			if (level == 0) {
				return NULL;
			}
		}
	}
}

static uint32_t get_num_const_args(zend_op_array *fbc, zend_op *opline, uint32_t num_args_passed) {
	unsigned level = 0;
	uint32_t num_const_args = 0;

	/* Parameter default values are constant arguments */
	if (num_args_passed < fbc->num_args) {
		num_const_args += fbc->num_args - num_args_passed;
	}

	/* Count explicitly passed constant arguments */
	for (;;) {
		opline++;
		if (is_init_opline(opline)) {
			level++;
		} else if (is_call_opline(opline)) {
			if (level == 0) {
				return num_const_args;
			}
			level--;
		} else if (opline->opcode == ZEND_SEND_VAL && opline->op1_type == IS_CONST) {
			num_const_args++;
		}
	}
}

static inline_info *find_inlinable_calls(
		zend_op_array *op_array, zend_optimizer_ctx *ctx, uint32_t pass) {
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;
	inline_info *info_head = NULL, *info_tail = NULL;
	for (; opline != end; opline++) {
		zend_op_array *fbc = NULL;

		/* Check for function call those target we can determine statically */
		if (opline->opcode == ZEND_INIT_FCALL) {
			zend_string *name = Z_STR(ZEND_OP2_LITERAL(opline));
			fbc = zend_hash_find_ptr(&ctx->script->function_table, name);
		} else if (opline->opcode == ZEND_INIT_STATIC_METHOD_CALL
				|| opline->opcode == ZEND_INIT_METHOD_CALL) {
			fbc = (zend_op_array *) zend_optimizer_get_called_func(
					ctx->script, op_array, opline, 0);
		}

		if (fbc) {
			uint32_t num_args_passed = opline->extended_value;
			uint32_t num_const_args = get_num_const_args(fbc, opline, num_args_passed);
			if (should_inline(op_array, fbc, num_const_args, pass)
					&& can_inline_scope(fbc, op_array)
					&& can_inline_from(fbc, num_args_passed, fbc != op_array)) {
				inline_info *info;
				zend_op *call_opline = find_call_opline(opline);
				if (!call_opline) {
					continue;
				}

				info = zend_arena_alloc(&ctx->arena, sizeof(inline_info));
				info->fbc = fbc;
				info->init_opline = opline;
				info->call_opline = call_opline;
				info->num_args_passed = num_args_passed;
				info->is_this_call = opline->opcode == ZEND_INIT_METHOD_CALL;

				/* We add one extra JMP for every RETURN and we drop RECVs for passed arguments */
				info->num_inlined_opcodes = fbc->last + num_returns(fbc) - num_args_passed;
				info->opnum_diff = opnum_diff(fbc, info->num_inlined_opcodes, info->is_this_call);

				if (call_opline->result_type == IS_UNUSED) {
					info->result_var_num = (uint32_t) -1;
					info->result_var_type = IS_UNUSED;
				} else {
					info->result_var_num = call_opline->result.var;
					info->result_var_type = call_opline->result_type;
				}

				info->next = NULL;
				if (info_tail) {
					info_tail->next = info;
				} else {
					info_head = info_tail = info;
				}

				/* For now we don't inline arguments to inlined functions */
				opline = call_opline;
			}
		}
	}
	return info_head;
}

static void copy_instr(
		zend_op_array *old_op_array, zend_op *new_opcodes,
		zend_op *old_opline, zend_op *new_opline, const merge_info *merge) {
#define TO_NEW(opline) \
		(opline - old_op_array->opcodes + new_opcodes) \
		+ merge->shiftlist[opline - old_op_array->opcodes]
	*new_opline = *old_opline;

	/* Adjust TMP/VAR offsets */
	if (new_opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
		new_opline->op1.var += merge->tmp_offset * sizeof(zval);
	}
	if (new_opline->op2_type & (IS_TMP_VAR|IS_VAR)) {
		new_opline->op2.var += merge->tmp_offset * sizeof(zval);
	}
	if (new_opline->result_type & (IS_TMP_VAR|IS_VAR)) {
		new_opline->result.var += merge->tmp_offset * sizeof(zval);
	}

	/* Adjust JMP offsets */
	switch (new_opline->opcode) {
		case ZEND_JMP:
		case ZEND_FAST_CALL:
			ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op1,
					TO_NEW(ZEND_OP1_JMP_ADDR(old_opline)));
			break;
		case ZEND_JMPZNZ:
			new_opline->extended_value = ZEND_OPLINE_TO_OFFSET(new_opline,
				TO_NEW(ZEND_OFFSET_TO_OPLINE(old_opline, old_opline->extended_value)));
			/* break missing intentionally */
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_ASSERT_CHECK:
			ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op2,
				TO_NEW(ZEND_OP2_JMP_ADDR(old_opline)));
			break;
		case ZEND_CATCH:
			if (!new_opline->result.num) {
				new_opline->extended_value = ZEND_OPLINE_TO_OFFSET(new_opline,
					TO_NEW(ZEND_OFFSET_TO_OPLINE(old_opline, old_opline->extended_value)));
			}
			break;
		case ZEND_DECLARE_ANON_CLASS:
		case ZEND_DECLARE_ANON_INHERITED_CLASS:
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
			new_opline->extended_value = ZEND_OPLINE_TO_OFFSET(new_opline,
				TO_NEW(ZEND_OFFSET_TO_OPLINE(old_opline, old_opline->extended_value)));
			break;
	} 

	/* Adjust live range offsets */
	if ((new_opline->opcode == ZEND_FREE || new_opline->opcode == ZEND_FE_FREE)
			&& (new_opline->extended_value & ZEND_FREE_ON_RETURN)) {
		new_opline->op2.num = merge->live_range_map[new_opline->op2.num];
	}
#undef TO_NEW
}

int32_t *create_shiftlists(zend_optimizer_ctx *ctx, zend_op_array *op_array, inline_info *info) {
#define NEXT() opline++; *shiftlist++ = shift;
	zend_op *opline = op_array->opcodes, *end = opline + op_array->last;
	int32_t *shiftlist_start = zend_arena_calloc(&ctx->arena, op_array->last, sizeof(int32_t));
	int32_t *shiftlist = shiftlist_start;
	int32_t shift = 0;

	while (opline != end) {
		if (!info || opline != info->init_opline) {
			NEXT();
			continue;
		}

		/* Skip INIT_FCALL */
		NEXT();
		shift--;

		if (info->is_this_call) {
			/* An ENSURE_HAVE_THIS will be inserted */
			shift++;
		}

		while (opline != info->call_opline) {
			NEXT();
		}

		/* Compute shiftlist for inlined function. */
		shift += opline - op_array->opcodes;
		{
			zend_op *opline = info->fbc->opcodes, *end = opline + info->fbc->last;
			int32_t *shiftlist = info->merge.shiftlist =
				zend_arena_calloc(&ctx->arena, info->fbc->last, sizeof(int32_t));
			int i;

			/* RECVs for passed arguments are skipped */
			for (i = 0; i < info->num_args_passed; i++) {
				NEXT();
				shift--;
			}

			while (opline != end) {
				NEXT();
				if ((opline-1)->opcode == ZEND_RETURN) {
					/* A JMP will be inserted after the RETURN */
					shift++;
				}
			}
		}
		shift -= opline - op_array->opcodes;

		/* Account for inlined opcodes */
		shift += info->fbc->last;

		/* UNSETs will be generated for each CV of the inlined function */
		shift += info->fbc->last_var;

		/* Skip DO_UCALL */
		NEXT();
		shift--;

		info = info->next;
	}
	return shiftlist_start;
#undef NEXT
}

static inline uint32_t translate_cv(uint32_t num, uint32_t cv_offset) {
	return num + cv_offset * sizeof(zval);
}

static void merge_opcodes(
		zend_op_array *op_array, inline_info *info,
		zend_op *new_opcodes, const merge_info *merge) {
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;
	zend_op *new_opline = new_opcodes;
	uint32_t cv_offset = op_array->last_var;
	uint32_t literal_offset = op_array->last_literal;
	uint32_t i;

	while (opline != end) {
		unsigned level = 0;
		if (!info || opline != info->init_opline) {
			copy_instr(op_array, new_opcodes, opline++, new_opline++, merge);
			continue;
		}

		/* Skip INIT_FCALL */
		opline++;

		if (info->is_this_call) {
			/* Insert an assertion that $this is defined */
			new_opline->opcode = ZEND_ENSURE_HAVE_THIS;
			SET_UNUSED(new_opline->op1);
			SET_UNUSED(new_opline->op2);
			SET_UNUSED(new_opline->result);
			new_opline->lineno = (opline - 1)->lineno;
			new_opline++;
		}

		while (opline != info->call_opline) {
			if (is_init_opline(opline)) {
				level++;
			} else if (is_call_opline(opline)) {
				level--;
			}

			copy_instr(op_array, new_opcodes, opline++, new_opline, merge);

			if ((new_opline->opcode == ZEND_SEND_VAL || new_opline->opcode == ZEND_SEND_VAR)
					&& level == 0) {
				/* Replace SEND with ASSIGN to argument variable. */
				uint32_t var_num = cv_offset + new_opline->op2.num - 1; /* op2 is 1-based argno */
				new_opline->opcode = ZEND_ASSIGN;
				COPY_NODE(new_opline->op2, new_opline->op1);
				new_opline->op1_type = IS_CV;
				new_opline->op1.var = (zend_uintptr_t) ZEND_CALL_VAR_NUM(NULL, var_num);
				SET_UNUSED(new_opline->result);
				OPT_STAT(inlining_arg_assigns)++;
			}

			new_opline++;
		}

		/* Insert inlined opcodes */
		{
			zend_op *opline = info->fbc->opcodes;
			zend_op *end = opline + info->fbc->last;
			zend_op *new_end = new_opline + info->num_inlined_opcodes;

			/* Skip RECVs for arguments that were passed */
			opline += info->num_args_passed;

			while (opline != end) {
				copy_instr(info->fbc, new_opcodes, opline++, new_opline, &info->merge);

				/* Adjust CV and CONST offsets */
				if (new_opline->op1_type == IS_CV) {
					new_opline->op1.var = translate_cv(new_opline->op1.var, cv_offset);
				} else if (new_opline->op1_type == IS_CONST) {
					if (info->fbc != op_array) {
						ZEND_PASS_TWO_UNDO_CONSTANT(info->fbc, new_opline->op1);
					}
					new_opline->op1.constant += literal_offset;
				}
				if (new_opline->op2_type == IS_CV) {
					new_opline->op2.var = translate_cv(new_opline->op2.var, cv_offset);
				} else if (new_opline->op2_type == IS_CONST) {
					if (info->fbc != op_array) {
						ZEND_PASS_TWO_UNDO_CONSTANT(info->fbc, new_opline->op2);
					}
					new_opline->op2.constant += literal_offset;
				}
				if (new_opline->result_type == IS_CV) {
					new_opline->result.var = translate_cv(new_opline->result.var, cv_offset);
				}

				if (new_opline->opcode == ZEND_RECV_INIT) {
					/* Convert RECV_INIT into ASSIGN */
					new_opline->opcode = ZEND_ASSIGN;
					COPY_NODE(new_opline->op1, new_opline->result);
					new_opline->result_type = IS_UNUSED;
					OPT_STAT(inlining_arg_assigns)++;
				} else if (new_opline->opcode == ZEND_RETURN) {
					if (info->result_var_num != (uint32_t) -1) {
						/* Convert RETURN into QM_ASSIGN to call result var */
						new_opline->opcode = ZEND_QM_ASSIGN;
						new_opline->result_type = info->result_var_type;
						if (info->result_var_type == IS_CV) {
							new_opline->result.var =
								info->result_var_num + cv_offset * sizeof(zval);
						} else {
							new_opline->result.var =
								info->result_var_num + merge->tmp_offset * sizeof(zval);
						}
					} else if (new_opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
						/* Convert RETURN into FREE if return value is unused */
						new_opline->opcode = ZEND_FREE;
						new_opline->extended_value = 0;
					} else {
						/* Convert RETURN into NOP if return value is unused and does not require
						 * freeing. We insert a NOP to preserve precise opcode counts. */
						MAKE_NOP(new_opline);
					}

					/* Insert jump to end of function */
					new_opline++;
					new_opline->opcode = ZEND_JMP;
					SET_UNUSED(new_opline->op1);
					SET_UNUSED(new_opline->op2);
					SET_UNUSED(new_opline->result);
					ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op1, new_end);
					// TODO We can directly elide the last (trivial) JMP
				}

				new_opline++;
			}
		}

		/* UNSET all CVs of the inlined function */
		for (i = 0; i < info->fbc->last_var; i++) {
			new_opline->opcode = ZEND_UNSET_VAR;
			new_opline->op1_type = IS_CV;
			new_opline->op1.var = (zend_uintptr_t) ZEND_CALL_VAR_NUM(NULL, cv_offset + i);
			SET_UNUSED(new_opline->op2);
			SET_UNUSED(new_opline->result);
			new_opline->extended_value = ZEND_FETCH_LOCAL | ZEND_QUICK_SET;
			new_opline++;
		}

		/* Skip DO_UCALL */
		opline++;

		cv_offset += info->fbc->last_var;
		literal_offset += info->fbc->last_literal;
		info = info->next;
	}
}

void merge_literals(zend_op_array *op_array, inline_info *info, zval *new_literals) {
	uint32_t cache_slot_offset = op_array->cache_size;
	memcpy(new_literals, op_array->literals, sizeof(zval) * op_array->last_literal);
	new_literals += op_array->last_literal;
	while (info) {
		zval *literals = info->fbc->literals, *end = literals + info->fbc->last_literal;
		while (literals != end) {
			*new_literals = *literals;
			zval_copy_ctor(new_literals);
			if (Z_CACHE_SLOT_P(new_literals) != (uint32_t) -1) {
				Z_CACHE_SLOT_P(new_literals) += cache_slot_offset;
			}
			new_literals++;
			literals++;
		}
		cache_slot_offset += info->fbc->cache_size;
		info = info->next;
	}
}

static void merge_cvs(zend_op_array *op_array, inline_info *info, zend_string **new_vars) {
	memcpy(new_vars, op_array->vars, sizeof(zend_string *) * op_array->last_var);
	new_vars += op_array->last_var;
	while (info) {
		int i;
		for (i = 0; i < info->fbc->last_var; i++) {
			/* We need to keep the original name for undefined variable notices and closure
			 * binding */
			*new_vars++ = zend_string_copy(info->fbc->vars[i]);
		}
		info = info->next;
	}
}

static inline void copy_live_range(
		zend_live_range *new_live_range, const zend_live_range *old_live_range,
		const zend_live_range *new_live_range_start, const zend_live_range *old_live_range_start,
		merge_info *merge) {
	merge->live_range_map[old_live_range - old_live_range_start] =
		(new_live_range - new_live_range_start);
	*new_live_range = *old_live_range;
	new_live_range->var += merge->tmp_offset * sizeof(zval);
	new_live_range->start += merge->shiftlist[new_live_range->start];
	new_live_range->end += merge->shiftlist[new_live_range->end];
}

static void merge_live_ranges(
		zend_optimizer_ctx *ctx, zend_op_array *op_array,
		inline_info *info, zend_live_range *new_live_range_start, merge_info *merge) {
#define TO_NEW(opnum, merge) (opnum + (merge)->shiftlist[opnum])
	zend_live_range *new_live_range = new_live_range_start;
	zend_live_range *live_range = op_array->live_range;
	zend_live_range *end = live_range + op_array->last_live_range;

	merge->live_range_map = zend_arena_calloc(
		&ctx->arena, op_array->last_live_range, sizeof(int32_t));
	while (info) {
		if (info->fbc->last_live_range) {
			/* Live ranges must be sorted by start */
			uint32_t i, limit = TO_NEW(info->fbc->live_range->start, &info->merge);
			while (live_range != end && TO_NEW(live_range->start, merge) < limit) {
				copy_live_range(new_live_range++, live_range++,
					new_live_range_start, op_array->live_range, merge);
			}

			info->merge.live_range_map = zend_arena_calloc(
				&ctx->arena, info->fbc->last_live_range, sizeof(int32_t));
			for (i = 0; i < info->fbc->last_live_range; i++) {
				copy_live_range(new_live_range++, &info->fbc->live_range[i],
					new_live_range_start, info->fbc->live_range, &info->merge);
			}
		}
		info = info->next;
	}
	while (live_range != end) {
		copy_live_range(new_live_range++, live_range++,
			new_live_range_start, op_array->live_range, merge);
	}
#undef TO_NEW
}

static inline void copy_try_catch(
		zend_try_catch_element *new_try_catch, zend_try_catch_element *old_try_catch,
		const int32_t *shiftlist) {
	*new_try_catch = *old_try_catch;
	new_try_catch->try_op += shiftlist[new_try_catch->try_op];
	new_try_catch->catch_op += shiftlist[new_try_catch->catch_op];
	if (new_try_catch->finally_op) {
		new_try_catch->finally_op += shiftlist[new_try_catch->finally_op];
		new_try_catch->finally_end += shiftlist[new_try_catch->finally_end];
	}
}

static void merge_try_catch(
		zend_op_array *op_array, inline_info *info, zend_try_catch_element *new_try_catch,
		int32_t *shiftlist) {
#define TO_NEW(opnum, shiftlist) (opnum + shiftlist[opnum])
	zend_try_catch_element *try_catch = op_array->try_catch_array;
	zend_try_catch_element *end = try_catch + op_array->last_try_catch;
	while (info) {
		if (info->fbc->last_try_catch) {
			uint32_t i, limit = TO_NEW(info->fbc->try_catch_array->try_op, info->merge.shiftlist);
			while (try_catch != end && TO_NEW(try_catch->try_op, shiftlist) < limit) {
				copy_try_catch(new_try_catch++, try_catch++, shiftlist);
			}
			for (i = 0; i < info->fbc->last_try_catch; i++) {
				copy_try_catch(new_try_catch++, &info->fbc->try_catch_array[i],
					info->merge.shiftlist);
			}
		}
		info = info->next;
	}
	while (try_catch != end) {
		copy_try_catch(new_try_catch++, try_catch++, shiftlist);
	}
#undef TO_NEW
}

static void inline_calls(zend_optimizer_ctx *ctx, zend_op_array *op_array, inline_info *info) {
	merge_info merge;
	uint32_t new_num_opcodes = op_array->last;
	uint32_t new_num_literals = op_array->last_literal;
	uint32_t new_num_cvs = op_array->last_var;
	uint32_t new_num_tmps = op_array->T;
	uint32_t new_num_cache_slots = op_array->cache_size;
	uint32_t new_num_live_ranges = op_array->last_live_range;
	uint32_t new_num_try_catch = op_array->last_try_catch;
	uint32_t tmp_offset;

	/* Compute new sizes for op_array structures */
	inline_info *cur;
	for (cur = info; cur; cur = cur->next) {
		new_num_opcodes += cur->opnum_diff;
		new_num_literals += cur->fbc->last_literal;
		new_num_cvs += cur->fbc->last_var;
		new_num_tmps += cur->fbc->T;
		new_num_cache_slots += cur->fbc->cache_size;
		new_num_live_ranges += cur->fbc->last_live_range;
		new_num_try_catch += cur->fbc->last_try_catch;

		OPT_STAT(inlining_funcs)++;
		OPT_STAT(inlining_instrs) += cur->fbc->last;
	}

	/* Compute TMP base offsets and this_vars */
	merge.tmp_offset = new_num_cvs - op_array->last_var;
	tmp_offset = new_num_cvs + op_array->T;
	for (cur = info; cur; cur = cur->next) {
		cur->merge.tmp_offset = tmp_offset - cur->fbc->last_var;
		tmp_offset += cur->fbc->T;
	}

	{
		zend_op *new_opcodes = ecalloc(new_num_opcodes, sizeof(zend_op));
		zend_string **new_vars = ecalloc(new_num_cvs, sizeof(zend_string*));
		zval *new_literals = ecalloc(new_num_literals, sizeof(zval));

		/* Shiftlists must be created first */
		merge.shiftlist = create_shiftlists(ctx, op_array, info);

		/* Live ranges have to be merged before merging opcodes,
		 * because it also computes the live range map. */
		if (new_num_live_ranges) {
			zend_live_range *new_live_ranges =
				ecalloc(new_num_live_ranges, sizeof(zend_live_range));
			merge_live_ranges(ctx, op_array, info, new_live_ranges, &merge);
			efree(op_array->live_range);
			op_array->live_range = new_live_ranges;
			op_array->last_live_range = new_num_live_ranges;
		}

		merge_opcodes(op_array, info, new_opcodes, &merge);
		merge_cvs(op_array, info, new_vars);
		merge_literals(op_array, info, new_literals);

		if (new_num_try_catch) {
			zend_try_catch_element *new_try_catch =
				ecalloc(new_num_try_catch, sizeof(zend_try_catch_element));
			merge_try_catch(op_array, info, new_try_catch, merge.shiftlist);
			efree(op_array->try_catch_array);
			op_array->try_catch_array = new_try_catch;
			op_array->last_try_catch = new_num_try_catch;
		}

		efree(op_array->opcodes);
		op_array->opcodes = new_opcodes;
		op_array->last = new_num_opcodes;

		efree(op_array->vars);
		op_array->vars = new_vars;
		op_array->last_var = new_num_cvs;

		efree(op_array->literals);
		op_array->literals = new_literals;
		op_array->last_literal = new_num_literals;

		op_array->T = new_num_tmps;
		op_array->cache_size = new_num_cache_slots;
	}
	// TODO add/extend live range for return temporary
}

void zend_optimize_inlining(zend_op_array *op_array, zend_optimizer_ctx *ctx) {
	void *checkpoint;
	inline_info *info;
	int i;

	/* Don't inline into main script -- $GLOBALS is too volatile */
	if (op_array == &ctx->script->main_op_array) {
		return;
	}

	if (!can_inline_into(op_array)) {
		return;
	}

	checkpoint = zend_arena_checkpoint(ctx->arena);
	for (i = 0; i < 1; i++) {
		info = find_inlinable_calls(op_array, ctx, i);
		if (info) {
			inline_calls(ctx, op_array, info);
		}
	}

	zend_arena_release(&ctx->arena, checkpoint);
}
