#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa/helpers.h"
#include "Optimizer/ssa/instructions.h"
#include "Optimizer/statistics.h"

#if 0
#define SCP_DEBUG(...) php_printf(__VA_ARGS__)
#else
#define SCP_DEBUG(...)
#endif

typedef struct _scp_ctx {
	zend_op_array *op_array;
	zend_ssa *ssa;
	zend_bitset var_worklist;
	zend_bitset block_worklist;
	zend_bitset executable_blocks;
	zend_bitset feasible_edges; /* Encoding: 2 bits per block, one for each successor */
	uint32_t var_worklist_len;
	uint32_t block_worklist_len;
	zval *values;
	zval top;
	zval bot;
} scp_ctx;

#define TOP ((zend_uchar)-1) 
#define BOT ((zend_uchar)-2)
#define IS_TOP(zv) (Z_TYPE_P(zv) == TOP)
#define IS_BOT(zv) (Z_TYPE_P(zv) == BOT)

#define MAKE_TOP(zv) (Z_TYPE_INFO_P(zv) = TOP)
#define MAKE_BOT(zv) (Z_TYPE_INFO_P(zv) = BOT)

static inline zend_bool value_known(zval *zv) {
	return !IS_TOP(zv) && !IS_BOT(zv);
}

/* Sets new value for variable and ensures that it is lower or equal
 * the previous one in the constant propagation lattice. */
static void set_value(scp_ctx *ctx, int var, zval *new) {
	zval *value = &ctx->values[var];
	if (IS_BOT(value) || IS_TOP(new)) {
		return;
	}

	if (IS_BOT(new)) {
		SCP_DEBUG("Lowering var %d to BOT\n", var);
	} else {
		SCP_DEBUG("Lowering var %d to %Z\n", var, new);
	}

	if (IS_TOP(value) || IS_BOT(new)) {
		zval_ptr_dtor_nogc(value);
		ZVAL_COPY(value, new);
		zend_bitset_incl(ctx->var_worklist, var);
		return;
	}
#if ZEND_DEBUG
	ZEND_ASSERT(zend_is_identical(value, new));
#endif
}

static zval *get_op1_value(scp_ctx *ctx, zend_op *opline, zend_ssa_op *ssa_op) {
	if (opline->op1_type == IS_CONST) {
		return CT_CONSTANT_EX(ctx->op_array, opline->op1.constant);
	} else if (ssa_op->op1_use != -1) {
		return &ctx->values[ssa_op->op1_use];
	} else {
		return &ctx->top;
	}
}
static zval *get_op2_value(scp_ctx *ctx, zend_op *opline, zend_ssa_op *ssa_op) {
	if (opline->op2_type == IS_CONST) {
		return CT_CONSTANT_EX(ctx->op_array, opline->op2.constant);
	} else if (ssa_op->op2_use != -1) {
		return &ctx->values[ssa_op->op2_use];
	} else {
		return &ctx->top;
	}
}

static zend_bool can_replace_op1(zend_op *opline, zend_ssa_op *ssa_op) {
	switch (opline->opcode) {
		case ZEND_PRE_INC:
		case ZEND_PRE_DEC:
		case ZEND_PRE_INC_OBJ:
		case ZEND_PRE_DEC_OBJ:
		case ZEND_POST_INC:
		case ZEND_POST_DEC:
		case ZEND_POST_INC_OBJ:
		case ZEND_POST_DEC_OBJ:
		case ZEND_ASSIGN:
		case ZEND_ASSIGN_REF:
		case ZEND_ASSIGN_DIM:
		case ZEND_ASSIGN_OBJ:
		case ZEND_ASSIGN_ADD:
		case ZEND_ASSIGN_SUB:
		case ZEND_ASSIGN_MUL:
		case ZEND_ASSIGN_DIV:
		case ZEND_ASSIGN_MOD:
		case ZEND_ASSIGN_SL:
		case ZEND_ASSIGN_SR:
		case ZEND_ASSIGN_CONCAT:
		case ZEND_ASSIGN_BW_OR:
		case ZEND_ASSIGN_BW_AND:
		case ZEND_ASSIGN_BW_XOR:
		case ZEND_ASSIGN_POW:
		case ZEND_FETCH_DIM_W:
		case ZEND_FETCH_DIM_RW:
		case ZEND_FETCH_DIM_UNSET:
		case ZEND_FETCH_DIM_FUNC_ARG:
		case ZEND_FETCH_OBJ_W:
		case ZEND_FETCH_OBJ_RW:
		case ZEND_FETCH_OBJ_UNSET:
		case ZEND_FETCH_OBJ_FUNC_ARG:
		case ZEND_UNSET_VAR:
		case ZEND_UNSET_DIM:
		case ZEND_UNSET_OBJ:
		// TODO are the following correct?
		case ZEND_SEND_REF:
		case ZEND_SEND_VAR_EX:
		case ZEND_SEND_UNPACK:
		case ZEND_SEND_ARRAY:
		case ZEND_SEND_USER:
		case ZEND_FE_RESET_RW:
			return 0;
		/* Do not accept CONST */
		case ZEND_VERIFY_ABSTRACT_CLASS:
		case ZEND_ADD_INTERFACE:
		case ZEND_ADD_TRAIT:
		case ZEND_BIND_TRAITS:
			return 0;
		case ZEND_ISSET_ISEMPTY_VAR:
			/* CV has special meaning here - cannot simply be replaced */
			// TODO We should be marking non-quick-set as TOO_DYNAMIC
			return (opline->extended_value & ZEND_QUICK_SET) == 0;
		case ZEND_INIT_ARRAY:
		case ZEND_ADD_ARRAY_ELEMENT:
			return !(opline->extended_value & ZEND_ARRAY_ELEMENT_REF);
		default:
			if (ssa_op->op1_def != -1) {
				ZEND_ASSERT(0);
				return 0;
			}
	}

	return 1;
}

static zend_bool can_replace_op2(zend_op *opline, zend_ssa_op *ssa_op) {
	switch (opline->opcode) {
		/* Do not accept CONST */
		case ZEND_DECLARE_INHERITED_CLASS:
		case ZEND_DECLARE_INHERITED_CLASS_DELAYED:
		case ZEND_DECLARE_ANON_INHERITED_CLASS:
		case ZEND_BIND_LEXICAL:
			return 0;
	}
	return 1;
}

static zend_bool try_replace_op1(scp_ctx *ctx, int var, zend_op *opline, zend_ssa_op *ssa_op) {
	if (ssa_op->op1_use == var && can_replace_op1(opline, ssa_op)) {
		zval value;
		ZVAL_DUP(&value, &ctx->values[var]); // TODO
		if (zend_optimizer_update_op1_const(ctx->op_array, opline, &value)) {
			return 1;
		}
	}
	return 0;
}
static zend_bool try_replace_op2(scp_ctx *ctx, int var, zend_op *opline, zend_ssa_op *ssa_op) {
	if (ssa_op->op2_use == var && can_replace_op2(opline, ssa_op)) {
		zval value;
		ZVAL_DUP(&value, &ctx->values[var]); // TODO
		if (zend_optimizer_update_op2_const(ctx->op_array, opline, &value)) {
			return 1;
		}
	}
	return 0;
}

static inline int ct_eval_binary(zval *result, zend_uchar opcode, zval *op1, zval *op2) {
	binary_op_type binary_op = get_binary_op(opcode);
	int retval;

	switch (opcode) {
		case ZEND_SR:
		case ZEND_SL:
			if (zval_get_long(op2) < 0) {
				return FAILURE;
			}
			break;
		case ZEND_DIV:
		case ZEND_MOD:
			if (zval_get_long(op2) == 0) {
				return FAILURE;
			}
			/* break missing intentionally */
		case ZEND_SUB:
		case ZEND_MUL:
		case ZEND_POW:
		case ZEND_CONCAT:
		case ZEND_FAST_CONCAT:
			if (Z_TYPE_P(op1) == IS_ARRAY || Z_TYPE_P(op2) == IS_ARRAY) {
				return FAILURE;
			}
			break;
	}

	retval = binary_op(result, op1, op2);
	ZEND_ASSERT(retval == SUCCESS);
	return retval;
}

static inline int ct_eval_unary(zval *result, zend_uchar opcode, zval *op1) {
	unary_op_type unary_op = get_unary_op(opcode);
	int retval;

	if (opcode == ZEND_BW_NOT && Z_TYPE_P(op1) != IS_LONG
			&& Z_TYPE_P(op1) != IS_DOUBLE && Z_TYPE_P(op1) != IS_STRING) {
		return FAILURE;
	}

	retval = unary_op(result, op1);
	ZEND_ASSERT(retval == SUCCESS);
	return retval;
}

static inline int ct_eval_cast(zval *result, zend_op *opline, zval *op1) {
	ZVAL_COPY(result, op1);
	switch (opline->extended_value) {
		case IS_NULL:
			convert_to_null(result);
			return SUCCESS;
		case _IS_BOOL:
			convert_to_boolean(result);
			return SUCCESS;
		case IS_LONG:
			convert_to_long(result);
			return SUCCESS;
		case IS_DOUBLE:
			convert_to_double(result);
			return SUCCESS;
		case IS_STRING:
			if (Z_TYPE_P(op1) == IS_ARRAY) {
				zval_ptr_dtor_nogc(result);
				return FAILURE;
			}
			convert_to_string(result);
			return SUCCESS;
		case IS_ARRAY:
			convert_to_array(result);
			return SUCCESS;
		case IS_OBJECT:
			/* We do not support object literals */
			zval_ptr_dtor_nogc(result);
			return FAILURE;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}

static inline int ct_eval_strlen(zval *result, zend_op *opline, zval *op1) {
	zend_string *str;
	if (Z_TYPE_P(op1) == IS_ARRAY) {
		return FAILURE;
	}

	str = zval_get_string(op1);
	ZVAL_LONG(result, ZSTR_LEN(str));
	zend_string_release(str);
	return SUCCESS;
}

static inline int ct_eval_fetch_dim(zval *result, zend_op *opline, zval *op1, zval *op2) {
	if (Z_TYPE_P(op1) == IS_ARRAY) {
		zval *value = NULL;
		switch (Z_TYPE_P(op2)) {
			case IS_NULL:
				value = zend_hash_find(Z_ARR_P(op1), ZSTR_EMPTY_ALLOC());
				break;
			case IS_FALSE:
				value = zend_hash_index_find(Z_ARR_P(op1), 0);
				break;
			case IS_TRUE:
				value = zend_hash_index_find(Z_ARR_P(op1), 1);
				break;
			case IS_LONG:
				value = zend_hash_index_find(Z_ARR_P(op1), Z_LVAL_P(op2));
				break;
			case IS_DOUBLE:
				value = zend_hash_index_find(Z_ARR_P(op1), zend_dval_to_lval(Z_DVAL_P(op2)));
				break;
			case IS_STRING:
				value = zend_symtable_find(Z_ARR_P(op1), Z_STR_P(op2));
				break;
		}
		if (value) {
			ZVAL_COPY(result, value);
			return SUCCESS;
		}
	} else if (Z_TYPE_P(op1) == IS_STRING) {
		zend_long index;
		switch (Z_TYPE_P(op2)) {
			case IS_LONG:
				index = Z_LVAL_P(op2);
				break;
			case IS_STRING:
				if (IS_LONG != is_numeric_string(
						Z_STRVAL_P(op2), Z_STRLEN_P(op2), &index, NULL, 0)) {
					return FAILURE;
				}
				break;
			default:
				return FAILURE;
		}
		if (index >= 0 && index < Z_STRLEN_P(op1)) {
			ZVAL_STR(result, zend_string_init(&Z_STRVAL_P(op1)[index], 1, 0));
			return SUCCESS;
		}
	}
	return FAILURE;
}

static inline int ct_eval_incdec(zval *result, zend_uchar opcode, zval *op1) {
	ZVAL_COPY(result, op1);
	if (opcode == ZEND_PRE_INC || opcode == ZEND_POST_INC) {
		increment_function(result);
	} else {
		decrement_function(result);
	}
	return SUCCESS;
}

static inline int ct_eval_isset_isempty(zval *result, uint32_t extended_value, zval *op1) {
	if (!(extended_value & ZEND_QUICK_SET)) {
		return FAILURE;
	}

	if (extended_value & ZEND_ISSET) {
		ZVAL_BOOL(result, Z_TYPE_P(op1) != IS_NULL);
	} else {
		ZEND_ASSERT(extended_value & ZEND_ISEMPTY);
		ZVAL_BOOL(result, !zend_is_true(op1));
	}
	return SUCCESS;
}

static inline void ct_eval_type_check(zval *result, uint32_t type, zval *op1) {
	if (type == _IS_BOOL) {
		ZVAL_BOOL(result, Z_TYPE_P(op1) == IS_TRUE || Z_TYPE_P(op1) == IS_FALSE);
	} else {
		ZVAL_BOOL(result, Z_TYPE_P(op1) == type);
	}
}

#define SET_RESULT(op, zv) do { \
	if (ssa_op->op##_def >= 0) { \
		set_value(ctx, ssa_op->op##_def, zv); \
	} \
} while (0)
#define SET_RESULT_BOT(op) SET_RESULT(op, &ctx->bot)
#define SET_RESULT_TOP(op) SET_RESULT(op, &ctx->top)

#define SKIP_IF_TOP(op) if (IS_TOP(op)) break;

static void interp_instr(scp_ctx *ctx, zend_op *opline, zend_ssa_op *ssa_op) {
	zval *op1 = get_op1_value(ctx, opline, ssa_op);
	zval *op2 = get_op2_value(ctx, opline, ssa_op);
	zval zv; /* Declare one zval to hold result values */

	switch (opline->opcode) {
		case ZEND_ASSIGN:
			/* The value of op1 is irrelevant here, because we are overwriting it
			 * -- unless it can be a reference, in which case we propagate a BOT. */
			if (IS_BOT(op1) && ssa_op->op1_use >= 0
					&& (ctx->ssa->var_info[ssa_op->op1_use].type & MAY_BE_REF)) {
				SET_RESULT_BOT(op1);
			} else {
				SET_RESULT(op1, op2);
			}

			SET_RESULT(result, op2);
			return;
		case ZEND_TYPE_CHECK:
			/* We may be able to evaluate TYPE_CHECK based on type inference info,
			 * even if we don't know the precise value. */
			if (!value_known(op1)) {
				uint32_t type = ctx->ssa->var_info[ssa_op->op1_use].type;
				uint32_t expected_type = opline->extended_value == _IS_BOOL
					? (MAY_BE_TRUE|MAY_BE_FALSE) : (1 << opline->extended_value);
				if (!CAN_BE(type, expected_type)) {
					ZVAL_FALSE(&zv);
					SET_RESULT(result, &zv);
					return;
				} else if (MUST_BE(type, expected_type)
						   && opline->extended_value != IS_OBJECT
						   && opline->extended_value != IS_RESOURCE) {
					ZVAL_TRUE(&zv);
					SET_RESULT(result, &zv);
					return;
				}
			}
			break;
	}

	if (IS_BOT(op1) || IS_BOT(op2)) {
		/* If any operand is BOT, mark the result as BOT right away. */
		// TODO: There are special cases where we can get a result even if one operand is BOT
		SET_RESULT_BOT(result);
		SET_RESULT_BOT(op1);
		SET_RESULT_BOT(op2);
		return;
	}

	switch (opline->opcode) {
		case ZEND_ADD:
		case ZEND_SUB:
		case ZEND_MUL:
		case ZEND_DIV:
		case ZEND_MOD:
		case ZEND_POW:
		case ZEND_SL:
		case ZEND_SR:
		case ZEND_CONCAT:
		case ZEND_FAST_CONCAT:
		case ZEND_IS_EQUAL:
		case ZEND_IS_NOT_EQUAL:
		case ZEND_IS_SMALLER:
		case ZEND_IS_SMALLER_OR_EQUAL:
		case ZEND_IS_IDENTICAL:
		case ZEND_IS_NOT_IDENTICAL:
		case ZEND_BW_OR:
		case ZEND_BW_AND:
		case ZEND_BW_XOR:
		case ZEND_BOOL_XOR:
			if (IS_TOP(op1) || IS_TOP(op2)) {
				break;
			}

			if (ct_eval_binary(&zv, opline->opcode, op1, op2) == SUCCESS) {
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(result);
			break;
		case ZEND_ASSIGN_ADD:
		case ZEND_ASSIGN_SUB:
		case ZEND_ASSIGN_MUL:
		case ZEND_ASSIGN_DIV:
		case ZEND_ASSIGN_MOD:
		case ZEND_ASSIGN_SL:
		case ZEND_ASSIGN_SR:
		case ZEND_ASSIGN_CONCAT:
		case ZEND_ASSIGN_BW_OR:
		case ZEND_ASSIGN_BW_AND:
		case ZEND_ASSIGN_BW_XOR:
		case ZEND_ASSIGN_POW:
			/* Obj/dim compound assign */
			if (opline->extended_value) {
				SET_RESULT_BOT(op1);
				SET_RESULT_BOT(result);
				break;
			}

			if (IS_TOP(op1) || IS_TOP(op2)) {
				break;
			}

			if (ct_eval_binary(&zv, instr_get_compound_assign_op(opline), op1, op2) == SUCCESS) {
				SET_RESULT(op1, &zv);
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(result);
			break;
		case ZEND_PRE_INC:
		case ZEND_PRE_DEC:
			SKIP_IF_TOP(op1);
			if (ct_eval_incdec(&zv, opline->opcode, op1) == SUCCESS) {
				SET_RESULT(op1, &zv);
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(op1);
			SET_RESULT_BOT(result);
			break;
		case ZEND_POST_INC:
		case ZEND_POST_DEC:
			SKIP_IF_TOP(op1);
			SET_RESULT(result, op1);
			if (ct_eval_incdec(&zv, opline->opcode, op1) == SUCCESS) {
				SET_RESULT(op1, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(op1);
			break;
		case ZEND_BW_NOT:
		case ZEND_BOOL_NOT:
			SKIP_IF_TOP(op1);
			if (ct_eval_unary(&zv, opline->opcode, op1) == SUCCESS) {
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(result);
			break;
		case ZEND_CAST:
			SKIP_IF_TOP(op1);
			if (ct_eval_cast(&zv, opline, op1) == SUCCESS) {
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(result);
			break;
		case ZEND_STRLEN:
			SKIP_IF_TOP(op1);
			if (ct_eval_strlen(&zv, opline, op1) == SUCCESS) {
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(result);
			break;
		case ZEND_FETCH_DIM_R:
			if (IS_TOP(op1) || IS_TOP(op2)) {
				break;
			}

			if (ct_eval_fetch_dim(&zv, opline, op1, op2) == SUCCESS) {
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(result);
			break;
		case ZEND_QM_ASSIGN:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_FETCH_CLASS:
			SET_RESULT(result, op1);
			break;
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
			SKIP_IF_TOP(op1);
			ZVAL_BOOL(&zv, zend_is_true(op1));
			SET_RESULT(result, &zv);
			break;
		case ZEND_ISSET_ISEMPTY_VAR:
			if (ct_eval_isset_isempty(&zv, opline->extended_value, op1) == SUCCESS) {
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(result);
			break;
		case ZEND_TYPE_CHECK:
			SKIP_IF_TOP(op1);
			ct_eval_type_check(&zv, opline->extended_value, op1);
			SET_RESULT(result, &zv);
			zval_ptr_dtor_nogc(&zv);
			break;
		default:
		{
			/* If we have no explicit implementation return BOT */
			SET_RESULT_BOT(result);
			SET_RESULT_BOT(op1);
			SET_RESULT_BOT(op2);
			break;
		}
	}
}

/* Returns whether there is a successor */
static zend_bool get_feasible_successors(
		scp_ctx *ctx, zend_basic_block *block,
		zend_op *opline, zend_ssa_op *ssa_op, zend_bool *suc) {
	zval *op1;

	/* Terminal block without sucessors */
	if (block->successors[0] < 0) {
		return 0;
	}

	/* Unconditional jump */
	if (block->successors[1] < 0) {
		suc[0] = 1;
		return 1;
	}

	/* We can't determine the branch target at compile-time for these */
	switch (opline->opcode) {
		case ZEND_ASSERT_CHECK:
		case ZEND_CATCH:
		case ZEND_DECLARE_ANON_CLASS:
		case ZEND_DECLARE_ANON_INHERITED_CLASS:
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
		case ZEND_NEW:
		// TODO For these two we could consider empty arrays
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
			suc[0] = 1;
			suc[1] = 1;
			return 1;
	}

	op1 = get_op1_value(ctx, opline, ssa_op);

	/* Branch target not yet known */
	if (IS_TOP(op1)) {
		return 0;
	}

	/* Branch target can be either one */
	if (IS_BOT(op1)) {
		suc[0] = 1;
		suc[1] = 1;
		return 1;
	}

	switch (opline->opcode) {
		case ZEND_JMPZ:
		case ZEND_JMPZNZ:
		case ZEND_JMPZ_EX:
			suc[zend_is_true(op1)] = 1;
			break;
		case ZEND_JMPNZ:
		case ZEND_JMPNZ_EX:
		case ZEND_JMP_SET:
			suc[!zend_is_true(op1)] = 1;
			break;
		case ZEND_COALESCE:
			suc[Z_TYPE_P(op1) == IS_NULL] = 1;
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return 1;
}

static void join_phi_values(zval *a, zval *b) {
	if (IS_BOT(a) || IS_TOP(b)) {
		return;
	}
	if (IS_TOP(a)) {
		zval_ptr_dtor_nogc(a);
		ZVAL_COPY(a, b);
		return;
	}
	if (IS_BOT(b) || !zend_is_identical(a, b)) {
		zval_ptr_dtor_nogc(a);
		MAKE_BOT(a);
	}
}

static zend_bool is_edge_feasible(scp_ctx *ctx, int from, int to) {
	zend_basic_block *block = &ctx->ssa->cfg.blocks[from];
	int suc;
	if (block->successors[0] == to) {
		suc = 0;
	} else if (block->successors[1] == to) {
		suc = 1;
	} else {
		ZEND_ASSERT(0);
	}
	return zend_bitset_in(ctx->feasible_edges, 2 * from + suc);
}

static void handle_phi(scp_ctx *ctx, zend_ssa_phi *phi) {
	zend_ssa *ssa = ctx->ssa;
	ZEND_ASSERT(phi->ssa_var >= 0);
	if (!IS_BOT(&ctx->values[phi->ssa_var])) {
		zend_basic_block *block = &ssa->cfg.blocks[phi->block];
		int *predecessors = &ssa->cfg.predecessors[block->predecessor_offset];

		int i;
		zval value;
		MAKE_TOP(&value);
		SCP_DEBUG("Handling PHI(");
		for (i = 0; i < block->predecessors_count; i++) {
			if (is_edge_feasible(ctx, predecessors[i], phi->block)) {
				SCP_DEBUG("val, ");
				join_phi_values(&value, &ctx->values[phi->sources[i]]);
			} else {
				SCP_DEBUG("--, ");
			}
		}
		SCP_DEBUG(")\n");

		set_value(ctx, phi->ssa_var, &value);
		zval_ptr_dtor_nogc(&value);
	}
}

static void mark_edge_feasible(scp_ctx *ctx, int from, int to, int suc_num) {
	int edge = from * 2 + suc_num;
	if (zend_bitset_in(ctx->feasible_edges, edge)) {
		/* We already handled this edge */
		return;
	}

	SCP_DEBUG("Marking edge %d->%d (successor %d) feasible\n", from, to, suc_num);
	zend_bitset_incl(ctx->feasible_edges, edge);

	if (!zend_bitset_in(ctx->executable_blocks, to)) {
		SCP_DEBUG("Marking block %d executable\n", to);
		zend_bitset_incl(ctx->executable_blocks, to);
		zend_bitset_incl(ctx->block_worklist, to);
	} else {
		/* Block is already executable, only a new edge became feasible.
		 * Reevaluate phi nodes to account for changed source operands. */
		zend_ssa_block *ssa_block = &ctx->ssa->blocks[to];
		zend_ssa_phi *phi;
		for (phi = ssa_block->phis; phi; phi = phi->next) {
			handle_phi(ctx, phi);
		}
	}
}

static void handle_instr(scp_ctx *ctx, int block_num, zend_op *opline, zend_ssa_op *ssa_op) {
	zend_basic_block *block = &ctx->ssa->cfg.blocks[block_num];
	interp_instr(ctx, opline, ssa_op);

	if (block->end == opline - ctx->op_array->opcodes) {
		zend_bool suc[2] = {0};
		if (get_feasible_successors(ctx, block, opline, ssa_op, suc)) {
			if (suc[0]) {
				mark_edge_feasible(ctx, block_num, block->successors[0], 0);
			}
			if (suc[1]) {
				mark_edge_feasible(ctx, block_num, block->successors[1], 1);
			}
		}
	}
}

static void scp_solve(scp_ctx *ctx) {
	zend_ssa *ssa = ctx->ssa;
	SCP_DEBUG("Start SCP solve\n");
	while (!zend_bitset_empty(ctx->var_worklist, ctx->var_worklist_len)
		|| !zend_bitset_empty(ctx->block_worklist, ctx->block_worklist_len)
	) {
		int i;
		while ((i = zend_bitset_pop_first(ctx->var_worklist, ctx->var_worklist_len)) >= 0) {
			zend_ssa_var *var = &ssa->vars[i];

#if 0
			int k;
			for (k = 0; k < ssa->vars_count; ++k) {
				zval *value = &ctx->values[k];
				if (Z_TYPE_P(value) == BOT) {
					php_printf("BOT\n");
				} else if (Z_TYPE_P(value) == TOP) {
					php_printf("TOP\n");
				} else {
					zend_print_zval_r(value, 0);
					php_printf("\n");
				}
			}
			php_printf("--- on %d\n", i);
#endif

			{
				int use;
				FOREACH_USE(var, use) {
					int block_num = ssa->cfg.map[use];
					if (zend_bitset_in(ctx->executable_blocks, block_num)) {
						handle_instr(ctx, block_num, &ctx->op_array->opcodes[use], &ssa->ops[use]);
					}
				} FOREACH_USE_END();
			}

			{
				zend_ssa_phi *phi;
				FOREACH_PHI_USE(var, phi) {
					handle_phi(ctx, phi);
				} FOREACH_PHI_USE_END();
			}
		}

		while ((i = zend_bitset_pop_first(ctx->block_worklist, ctx->block_worklist_len)) >= 0) {
			/* This block is now live. Interpret phis and instructions in it. */
			zend_basic_block *block = &ssa->cfg.blocks[i];
			zend_ssa_block *ssa_block = &ssa->blocks[i];

			SCP_DEBUG("Pop block %d from worklist\n", i);

			{
				zend_ssa_phi *phi;
				for (phi = ssa_block->phis; phi; phi = phi->next) {
					handle_phi(ctx, phi);
				}
			}

			{
				int j;
				for (j = block->start; j <= block->end; j++) {
					handle_instr(ctx, i, &ctx->op_array->opcodes[j], &ssa->ops[j]);
				}
			}
		}
	}
}

/* Removes instructions inside dead blocks. We don't remove the actual blocks, as modifying the
 * CFG would be overly painful at this point. */
static void eliminate_dead_blocks(scp_ctx *ctx) {
	zend_ssa *ssa = ctx->ssa;
	int i;
	for (i = 0; i < ssa->cfg.blocks_count; i++) {
		if (!zend_bitset_in(ctx->executable_blocks, i)) {
			zend_basic_block *block = &ssa->cfg.blocks[i];
			int j;
			OPT_STAT(scp_dead_blocks)++;
			for (j = block->start; j <= block->end; j++) {
				remove_instr_with_defs(ssa, &ctx->op_array->opcodes[j], &ssa->ops[j]);
			}
		}
	}
}

static void replace_constant_operands(scp_ctx *ctx) {
	int i;
	zend_ssa *ssa = ctx->ssa;
	for (i = 0; i < ssa->vars_count; ++i) {
		zend_ssa_var *var = &ssa->vars[i];
		int use;
		if (!value_known(&ctx->values[i])) {
			continue;
		}
		OPT_STAT(scp_const_vars)++;

		FOREACH_USE(var, use) {
			zend_op *opline = &ctx->op_array->opcodes[use];
			zend_ssa_op *ssa_op = &ssa->ops[use];
			if (try_replace_op1(ctx, i, opline, ssa_op)) {
				ZEND_ASSERT(ssa_op->op1_def == -1);
				remove_op1_use(ssa, ssa_op);
			}
			if (try_replace_op2(ctx, i, opline, ssa_op)) {
				if (ssa_op->op2_def >= 0) {
					rename_var_uses(ssa, ssa_op->op2_def, ssa_op->op2_use);
					remove_op2_def(ssa, ssa_op);
				}
				remove_op2_use(ssa, ssa_op);
			}
		} FOREACH_USE_END();
	}
}

/* This is a basic DCE pass we run after SCP. It only works on those instructions those result
 * value(s) were determined by SCP. It removes dead computational instructions and converts
 * CV-affecting instructions into CONST ASSIGNs. This basic DCE is performed for two reasons:
 * a) During operand replacement we eliminate FREEs. The corresponding computational instructions
 *    must be removed to avoid leaks. This way SCP can run independently of the full DCE pass.
 * b) The main DCE pass relies on type analysis to determine whether instructions have side-effects
 *    and can't be DCEd. This means that it will not be able collect all instructions rendered dead
 *    by SCP, because they may have potentially side-effecting types, but the actual values are
 *    not. As such doing DCE here will allow us to eliminate more dead code in combination. */
static void eliminate_dead_instructions(scp_ctx *ctx) {
	zend_ssa *ssa = ctx->ssa;
	zend_op_array *op_array = ctx->op_array;
	int i;

	for (i = 0; i < ssa->vars_count; ++i) {
		zend_ssa_var *var = &ssa->vars[i];
		if (value_known(&ctx->values[i]) && var->definition >= 0) {
			zend_op *opline = &op_array->opcodes[var->definition];
			zend_ssa_op *ssa_op = &ssa->ops[var->definition];
			if (ssa_op->result_def >= 0 && ssa_op->op1_def < 0 && ssa_op->op2_def < 0
					&& !var_used(&ssa->vars[ssa_op->result_def])) {
				/* Ordinary computational instruction -> remove it */
				OPT_STAT(scp_dead_instrs)++;
				remove_result_def(ssa, ssa_op);
				remove_instr(ssa, opline, ssa_op);
			} else if (opline->opcode != ZEND_ASSIGN && ssa_op->op1_def >= 0) {
				/* Compound assign or incdec -> convert to direct ASSIGN */
				zval *val = &ctx->values[ssa_op->op1_def];
				ZEND_ASSERT(value_known(val));
				OPT_STAT(scp_semi_dead_instrs)++;

				/* Destroy previous op2 */
				if (opline->op2_type == IS_CONST) {
					literal_dtor(&ZEND_OP2_LITERAL(opline));
				} else if (ssa_op->op2_use >= 0) {
					remove_op2_use(ssa, ssa_op);
				}

				/* Mark result unused, if possible */
				if (ssa_op->result_def >= 0 && !var_used(&ssa->vars[ssa_op->result_def])) {
					remove_result_def(ssa, ssa_op);
					opline->result_type = IS_VAR | EXT_TYPE_UNUSED;
				}

				/* Convert to ASSIGN */
				opline->opcode = ZEND_ASSIGN;
				opline->op2_type = IS_CONST;
				opline->op2.constant = zend_optimizer_add_literal(op_array, val);
				Z_TRY_ADDREF_P(val);
			}
		}
	}
}

void ssa_optimize_scp(zend_optimizer_ctx *opt_ctx, zend_op_array *op_array, zend_ssa *ssa) {
	int i, ssa_vars = ssa->vars_count;
	
	scp_ctx ctx;

	ctx.op_array = op_array;
	ctx.ssa = ssa;
	ctx.values = alloca(sizeof(zval) * ssa_vars);

	MAKE_TOP(&ctx.top);
	MAKE_BOT(&ctx.bot);

	ctx.var_worklist_len = zend_bitset_len(ssa_vars);
	ctx.var_worklist = alloca(sizeof(zend_ulong) * ctx.var_worklist_len);
	memset(ctx.var_worklist, 0, sizeof(zend_ulong) * ctx.var_worklist_len);

	ctx.block_worklist_len = zend_bitset_len(ssa->cfg.blocks_count);
	ctx.block_worklist = alloca(sizeof(zend_ulong) * ctx.block_worklist_len);
	memset(ctx.block_worklist, 0, sizeof(zend_ulong) * ctx.block_worklist_len);

	ctx.executable_blocks = alloca(sizeof(zend_ulong) * ctx.block_worklist_len);
	memset(ctx.executable_blocks, 0, sizeof(zend_ulong) * ctx.block_worklist_len);
	ctx.feasible_edges = alloca(sizeof(zend_ulong) * ctx.block_worklist_len * 2);
	memset(ctx.feasible_edges, 0, sizeof(zend_ulong) * ctx.block_worklist_len * 2);

	zend_bitset_incl(ctx.block_worklist, 0);
	zend_bitset_incl(ctx.executable_blocks, 0);

	i = 0;
	for (; i < op_array->last_var; ++i) {
		/* These are all undefined variables, which we have to mark BOT.
		 * Otherwise the undefined variable warning might not be preserved. */
		MAKE_BOT(&ctx.values[i]);
	}
	for (; i < ssa_vars; ++i) {
		if (ssa->var_info[i].type & MAY_BE_REF) {
			MAKE_BOT(&ctx.values[i]);
			continue;
		}
		MAKE_TOP(&ctx.values[i]);
	}

	scp_solve(&ctx);
	eliminate_dead_blocks(&ctx);
	replace_constant_operands(&ctx);
	eliminate_dead_instructions(&ctx);

	for (i = 0; i < ssa_vars; ++i) {
		zval_ptr_dtor_nogc(&ctx.values[i]);
	}
}
