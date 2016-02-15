#include "php.h"
#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/instructions.h"
#include "Optimizer/ssa/scdf.h"
#include "Optimizer/statistics.h"

#if 0
#define SCP_DEBUG(...) php_printf(__VA_ARGS__)
#else
#define SCP_DEBUG(...)
#endif

typedef struct _scp_ctx {
	scdf_ctx scdf;
	zend_op_array *op_array;
	zend_ssa *ssa;
	zend_call_info **call_map;
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
		scdf_add_to_worklist(&ctx->scdf, var);
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
		return NULL;
	}
}
static zval *get_op2_value(scp_ctx *ctx, zend_op *opline, zend_ssa_op *ssa_op) {
	if (opline->op2_type == IS_CONST) {
		return CT_CONSTANT_EX(ctx->op_array, opline->op2.constant);
	} else if (ssa_op->op2_use != -1) {
		return &ctx->values[ssa_op->op2_use];
	} else {
		return NULL;
	}
}

static zend_bool can_replace_op1(zend_op_array *op_array, zend_op *opline, zend_ssa_op *ssa_op) {
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
		case ZEND_UNSET_DIM:
		case ZEND_UNSET_OBJ:
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
		case ZEND_ROPE_ADD:
		case ZEND_ROPE_END:
		case ZEND_BIND_STATIC:
			return 0;
		case ZEND_UNSET_VAR:
		case ZEND_ISSET_ISEMPTY_VAR:
			/* CV has special meaning here - cannot simply be replaced */
			return (opline->extended_value & ZEND_QUICK_SET) == 0;
		case ZEND_INIT_ARRAY:
		case ZEND_ADD_ARRAY_ELEMENT:
			return !(opline->extended_value & ZEND_ARRAY_ELEMENT_REF);
		case ZEND_YIELD:
			return !(op_array->fn_flags & ZEND_ACC_RETURN_REFERENCE);
		default:
			if (ssa_op->op1_def != -1) {
				ZEND_ASSERT(0);
				return 0;
			}
	}

	return 1;
}

static zend_bool can_replace_op2(zend_op_array *op_array, zend_op *opline, zend_ssa_op *ssa_op) {
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
	if (ssa_op->op1_use == var && can_replace_op1(ctx->op_array, opline, ssa_op)) {
		zval value;
		ZVAL_DUP(&value, &ctx->values[var]); // TODO
		if (zend_optimizer_update_op1_const(ctx->op_array, opline, &value)) {
			return 1;
		}
	}
	return 0;
}
static zend_bool try_replace_op2(scp_ctx *ctx, int var, zend_op *opline, zend_ssa_op *ssa_op) {
	if (ssa_op->op2_use == var && can_replace_op2(ctx->op_array, opline, ssa_op)) {
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

static inline int ct_eval_cast(zval *result, uint32_t extended_value, zval *op1) {
	ZVAL_COPY(result, op1);
	switch (extended_value) {
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

static inline int zval_to_string_offset(zend_long *result, zval *op) {
	switch (Z_TYPE_P(op)) {
		case IS_LONG:
			*result = Z_LVAL_P(op);
			return SUCCESS;
		case IS_STRING:
			if (IS_LONG == is_numeric_string(
					Z_STRVAL_P(op), Z_STRLEN_P(op), result, NULL, 0)) {
				return SUCCESS;
			}
			return FAILURE;
		default:
			return FAILURE;
	}
}

static inline int fetch_array_elem(zval **result, zval *op1, zval *op2) {
	switch (Z_TYPE_P(op2)) {
		case IS_NULL:
			*result = zend_hash_find(Z_ARR_P(op1), ZSTR_EMPTY_ALLOC());
			return SUCCESS;
		case IS_FALSE:
			*result = zend_hash_index_find(Z_ARR_P(op1), 0);
			return SUCCESS;
		case IS_TRUE:
			*result = zend_hash_index_find(Z_ARR_P(op1), 1);
			return SUCCESS;
		case IS_LONG:
			*result = zend_hash_index_find(Z_ARR_P(op1), Z_LVAL_P(op2));
			return SUCCESS;
		case IS_DOUBLE:
			*result = zend_hash_index_find(Z_ARR_P(op1), zend_dval_to_lval(Z_DVAL_P(op2)));
			return SUCCESS;
		case IS_STRING:
			*result = zend_symtable_find(Z_ARR_P(op1), Z_STR_P(op2));
			return SUCCESS;
		default:
			return FAILURE;
	}
}

static inline int ct_eval_fetch_dim(zval *result, zend_op *opline, zval *op1, zval *op2) {
	if (Z_TYPE_P(op1) == IS_ARRAY) {
		zval *value;
		if (fetch_array_elem(&value, op1, op2) == SUCCESS && value) {
			ZVAL_COPY(result, value);
			return SUCCESS;
		}
	} else if (Z_TYPE_P(op1) == IS_STRING) {
		zend_long index;
		if (zval_to_string_offset(&index, op2) == FAILURE) {
			return FAILURE;
		}
		if (index >= 0 && index < Z_STRLEN_P(op1)) {
			ZVAL_STR(result, zend_string_init(&Z_STRVAL_P(op1)[index], 1, 0));
			return SUCCESS;
		}
	}
	return FAILURE;
}

static inline int ct_eval_isset_dim(zval *result, uint32_t extended_value, zval *op1, zval *op2) {
	if (Z_TYPE_P(op1) == IS_ARRAY) {
		zval *value;
		if (fetch_array_elem(&value, op1, op2) == FAILURE) {
			return FAILURE;
		}
		if (extended_value & ZEND_ISSET) {
			ZVAL_BOOL(result, value && Z_TYPE_P(value) != IS_NULL);
		} else {
			ZEND_ASSERT(extended_value & ZEND_ISEMPTY);
			ZVAL_BOOL(result, !value || !zend_is_true(value));
		}
		return SUCCESS;
	} else if (Z_TYPE_P(op1) == IS_STRING) {
		// TODO
		return FAILURE;
	} else {
		ZVAL_BOOL(result, extended_value != ZEND_ISSET);
		return SUCCESS;
	}
}

// TODO Avoid the copy_ctor
static inline int ct_eval_add_array_elem(zval *result, zval *value, zval *key) {
	if (!key) {
		if ((value = zend_hash_next_index_insert(Z_ARR_P(result), value))) {
			zval_copy_ctor(value);
			return SUCCESS;
		}
		return FAILURE;
	}

	switch (Z_TYPE_P(key)) {
		case IS_NULL:
			value = zend_hash_update(Z_ARR_P(result), ZSTR_EMPTY_ALLOC(), value);
			break;
		case IS_FALSE:
			value = zend_hash_index_update(Z_ARR_P(result), 0, value);
			break;
		case IS_TRUE:
			value = zend_hash_index_update(Z_ARR_P(result), 1, value);
			break;
		case IS_LONG:
			value = zend_hash_index_update(Z_ARR_P(result), Z_LVAL_P(key), value);
			break;
		case IS_DOUBLE:
			value = zend_hash_index_update(
				Z_ARR_P(result), zend_dval_to_lval(Z_DVAL_P(key)), value);
			break;
		case IS_STRING:
			value = zend_symtable_update(Z_ARR_P(result), Z_STR_P(key), value);
			break;
		default:
			return FAILURE;
	}

	zval_copy_ctor(value);
	return SUCCESS;
}

static inline int ct_eval_assign_dim(zval *result, zval *value, zval *key) {
	switch (Z_TYPE_P(result)) {
		case IS_NULL:
		case IS_FALSE:
			array_init(result);
			/* break missing intentionally */
		case IS_ARRAY:
			return ct_eval_add_array_elem(result, value, key);
		case IS_STRING:
			// TODO Before enabling this case, make sure ARRAY_DIM result op is correct
#if 0
			zend_long index;
			zend_string *new_str, *value_str;
			if (!key || Z_TYPE_P(value) == IS_ARRAY
					|| zval_to_string_offset(&index, key) == FAILURE || index < 0) {
				return FAILURE;
			}

			if (index >= Z_STRLEN_P(result)) {
				new_str = zend_string_alloc(index + 1, 0);
				memcpy(ZSTR_VAL(new_str), Z_STRVAL_P(result), Z_STRLEN_P(result));
				memset(ZSTR_VAL(new_str) + Z_STRLEN_P(result), ' ', index - Z_STRLEN_P(result));
				ZSTR_VAL(new_str)[index + 1] = 0;
			} else {
				new_str = zend_string_init(Z_STRVAL_P(result), Z_STRLEN_P(result), 0);
			}

			value_str = zval_get_string(value);
			ZVAL_STR(result, new_str);
			Z_STRVAL_P(result)[index] = ZSTR_VAL(value_str)[0];
			zend_string_release(value_str);
#endif
			return FAILURE;
		default:
			return FAILURE;
	}
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

static inline int ct_eval_func_call(
		zval *result, zend_string *name, uint32_t num_args, zval **args) {
	if (zend_string_equals_literal(name, "chr")) {
		zend_long c;
		if (num_args != 1 || Z_TYPE_P(args[0]) != IS_LONG) {
			return FAILURE;
		}

		c = Z_LVAL_P(args[0]) & 0xff;
		if (CG(one_char_string)[c]) {
			ZVAL_INTERNED_STR(result, CG(one_char_string)[c]);
		} else {
			ZVAL_NEW_STR(result, zend_string_alloc(1, 0));
			Z_STRVAL_P(result)[0] = (char)c;
			Z_STRVAL_P(result)[1] = '\0';
		}
		return SUCCESS;
	} else if (zend_string_equals_literal(name, "in_array")) {
		zval *val;
		if (num_args != 2 || Z_TYPE_P(args[1]) != IS_ARRAY) {
			return FAILURE;
		}

		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(args[1]), val) {
			if (fast_equal_check_function(val, args[0])) {
				ZVAL_TRUE(result);
				return SUCCESS;
			}
		} ZEND_HASH_FOREACH_END();
		ZVAL_FALSE(result);
		return SUCCESS;
	} else if (zend_string_equals_literal(name, "strpos")) {
		const char *found;
		if (num_args != 2 || Z_TYPE_P(args[0]) != IS_STRING || Z_TYPE_P(args[1]) != IS_STRING) {
			return FAILURE;
		}

		found = zend_memnstr(
			Z_STRVAL_P(args[0]), Z_STRVAL_P(args[1]), Z_STRLEN_P(args[1]),
			Z_STRVAL_P(args[0]) + Z_STRLEN_P(args[0]));
		if (found) {
			ZVAL_LONG(result, found - Z_STRVAL_P(args[0]));
		} else {
			ZVAL_FALSE(result);
		}
		return SUCCESS;
	} else if (zend_string_equals_literal(name, "count")) {
		if (num_args != 1 || Z_TYPE_P(args[0]) != IS_ARRAY) {
			return FAILURE;
		}

		ZVAL_LONG(result, zend_hash_num_elements(Z_ARRVAL_P(args[0])));
		return SUCCESS;
	} else if (zend_string_equals_literal(name, "array_key_exists")) {
		zval *value;
		if (num_args != 2 || Z_TYPE_P(args[1]) != IS_ARRAY ||
				(Z_TYPE_P(args[0]) != IS_LONG && Z_TYPE_P(args[0]) != IS_STRING
				 && Z_TYPE_P(args[0]) != IS_NULL)) {
			return FAILURE;
		}

		if (fetch_array_elem(&value, args[1], args[0])) {
			return FAILURE;
		}
		ZVAL_BOOL(result, value != NULL);
		return SUCCESS;
	}
	return FAILURE;
}

#define SET_RESULT(op, zv) do { \
	if (ssa_op->op##_def >= 0) { \
		set_value(ctx, ssa_op->op##_def, zv); \
	} \
} while (0)
#define SET_RESULT_BOT(op) SET_RESULT(op, &ctx->bot)
#define SET_RESULT_TOP(op) SET_RESULT(op, &ctx->top)

#define SKIP_IF_TOP(op) if (IS_TOP(op)) break;

static void visit_instr(void *void_ctx, zend_op *opline, zend_ssa_op *ssa_op) {
	scp_ctx *ctx = (scp_ctx *) void_ctx;
	zval *op1, *op2, zv; /* zv is a temporary to hold result values */

	if (opline->opcode == ZEND_OP_DATA) {
		opline--;
		ssa_op--;
	}

	op1 = get_op1_value(ctx, opline, ssa_op);
	op2 = get_op2_value(ctx, opline, ssa_op);

	switch (opline->opcode) {
		case ZEND_ASSIGN:
			/* The value of op1 is irrelevant here, because we are overwriting it
			 * -- unless it can be a reference, in which case we propagate a BOT. */
			if (IS_BOT(op1) && (ctx->ssa->var_info[ssa_op->op1_use].type & MAY_BE_REF)) {
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
		case ZEND_ASSIGN_DIM:
			/* If $a in $a[$b]=$c is UNDEF, treat it like NULL. There is no warning. */
			if ((ctx->ssa->var_info[ssa_op->op1_use].type & MAY_BE_ANY) == 0) {
				op1 = &EG(uninitialized_zval);
			}
			break;
		case ZEND_SEND_VAL:
		case ZEND_SEND_VAR:
		{
			/* If the value of a SEND for an ICALL changes, we need to reconsider the
			 * ICALL result value. Otherwise we can ignore the opcode. */
			zend_call_info *call = ctx->call_map[opline - ctx->op_array->opcodes];
			if (IS_TOP(op1) || !call || call->caller_call_opline->opcode != ZEND_DO_ICALL) {
				return;
			}

			opline = call->caller_call_opline;
			ssa_op = &ctx->ssa->ops[opline - ctx->op_array->opcodes];
			break;
		}
	}

	if ((op1 && IS_BOT(op1)) || (op2 && IS_BOT(op2))) {
		/* If any operand is BOT, mark the result as BOT right away.
		 * Exceptions to this rule are handled above. */
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
			SKIP_IF_TOP(op1);
			SKIP_IF_TOP(op2);

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

			SKIP_IF_TOP(op1);
			SKIP_IF_TOP(op2);

			if (ct_eval_binary(&zv, instr_get_compound_assign_op(opline), op1, op2) == SUCCESS) {
				SET_RESULT(op1, &zv);
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(op1);
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
			if (ct_eval_cast(&zv, opline->extended_value, op1) == SUCCESS) {
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(result);
			break;
		case ZEND_BOOL:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
			SKIP_IF_TOP(op1);
			ZVAL_BOOL(&zv, zend_is_true(op1));
			SET_RESULT(result, &zv);
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
			SKIP_IF_TOP(op1);
			SKIP_IF_TOP(op2);

			if (ct_eval_fetch_dim(&zv, opline, op1, op2) == SUCCESS) {
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(result);
			break;
		case ZEND_ISSET_ISEMPTY_DIM_OBJ:
			SKIP_IF_TOP(op1);
			SKIP_IF_TOP(op2);

			if (ct_eval_isset_dim(&zv, opline->extended_value, op1, op2) == SUCCESS) {
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(result);
			break;
		case ZEND_QM_ASSIGN:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
			SET_RESULT(result, op1);
			break;
		case ZEND_FETCH_CLASS:
			if (!op1) {
				SET_RESULT_BOT(result);
				break;
			}
			SET_RESULT(result, op1);
			break;
		case ZEND_ISSET_ISEMPTY_VAR:
			SKIP_IF_TOP(op1);
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
		case ZEND_INSTANCEOF:
			SKIP_IF_TOP(op1);
			ZVAL_FALSE(&zv);
			SET_RESULT(result, &zv);
			break;
		case ZEND_ROPE_INIT:
			SKIP_IF_TOP(op2);
			if (ct_eval_cast(&zv, IS_STRING, op2) == SUCCESS) {
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(result);
			break;
		case ZEND_ROPE_ADD:
		case ZEND_ROPE_END:
			SKIP_IF_TOP(op1);
			SKIP_IF_TOP(op2);
			if (ct_eval_binary(&zv, ZEND_CONCAT, op1, op2) == SUCCESS) {
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(result);
			break;
		case ZEND_INIT_ARRAY:
		case ZEND_ADD_ARRAY_ELEMENT:
		{
			zval *result = NULL;
			if (opline->extended_value & ZEND_ARRAY_ELEMENT_REF) {
				SET_RESULT_BOT(result);
				SET_RESULT_BOT(op1);
				break;
			}

			if (opline->opcode == ZEND_ADD_ARRAY_ELEMENT) {
				result = &ctx->values[ssa_op->result_use];
				if (IS_BOT(result)) {
					SET_RESULT_BOT(result);
					break;
				}
				SKIP_IF_TOP(result);
			}

			SKIP_IF_TOP(op1);
			if (op2) {
				SKIP_IF_TOP(op2);
			}

			if (result) {
				ZVAL_DUP(&zv, result);
			} else {
				array_init(&zv);
			}

			if (ct_eval_add_array_elem(&zv, op1, op2) == SUCCESS) {
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(result);
			zval_ptr_dtor_nogc(&zv);
			break;
		}
		case ZEND_ASSIGN_DIM:
		{
			zval *data = get_op1_value(ctx, opline+1, ssa_op+1);
			if (IS_BOT(data)) {
				SET_RESULT_BOT(op1);
				SET_RESULT_BOT(result);
				break;
			}

			SKIP_IF_TOP(data);
			SKIP_IF_TOP(op1);
			if (op2) {
				SKIP_IF_TOP(op2);
			}

			ZVAL_DUP(&zv, op1);
			if (ct_eval_assign_dim(&zv, data, op2) == SUCCESS) {
				SET_RESULT(result, data);
				SET_RESULT(op1, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}
			SET_RESULT_BOT(result);
			SET_RESULT_BOT(op1);
			zval_ptr_dtor_nogc(&zv);
			break;
		}
		case ZEND_DO_ICALL:
		{
			zend_call_info *call = ctx->call_map[opline - ctx->op_array->opcodes];
			zval *name = CT_CONSTANT_EX(ctx->op_array, call->caller_init_opline->op2.constant);
			zval *args[2] = {NULL};
			int i;

			/* We already know it can't be evaluated, don't bother checking again */
			if (ssa_op->result_def < 0 || IS_BOT(&ctx->values[ssa_op->result_def])) {
				break;
			}

			/* We're only interested in functions with one or two arguments right now */
			if (call->num_args == 0 || call->num_args > 2) {
				SET_RESULT_BOT(result);
				break;
			}

			for (i = 0; i < call->num_args; i++) {
				zend_op *opline = call->arg_info[i].opline;
				if (opline->opcode != ZEND_SEND_VAL && opline->opcode != ZEND_SEND_VAR) {
					SET_RESULT_BOT(result);
					return;
				}

				args[i] = get_op1_value(ctx, opline,
					&ctx->ssa->ops[opline - ctx->op_array->opcodes]);
				if (args[i]) {
					if (IS_BOT(args[i])) {
						SET_RESULT_BOT(result);
						return;
					} else if (IS_TOP(args[i])) {
						return;
					}
				}
			}

			/* We didn't get a BOT argument, so value stays the same */
			if (!IS_TOP(&ctx->values[ssa_op->result_def])) {
				break;
			}

			if (ct_eval_func_call(&zv, Z_STR_P(name), call->num_args, args) == SUCCESS) {
				SET_RESULT(result, &zv);
				zval_ptr_dtor_nogc(&zv);
				break;
			}

			/*fprintf(stderr, "%s\n", Z_STRVAL_P(name));
			if (args[1]) {
				php_printf("%s %Z %Z\n", Z_STRVAL_P(name), args[0], args[1]);
			} else {
				php_printf("%s %Z\n", Z_STRVAL_P(name), args[0]);
			}*/

			SET_RESULT_BOT(result);
			break;
		}
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
		void *void_ctx, zend_basic_block *block,
		zend_op *opline, zend_ssa_op *ssa_op, zend_bool *suc) {
	scp_ctx *ctx = (scp_ctx *) void_ctx;
	zval *op1;

	/* We can't determine the branch target at compile-time for these */
	switch (opline->opcode) {
		case ZEND_ASSERT_CHECK:
		case ZEND_CATCH:
		case ZEND_DECLARE_ANON_CLASS:
		case ZEND_DECLARE_ANON_INHERITED_CLASS:
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
		case ZEND_NEW:
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
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
			if (Z_TYPE_P(op1) == IS_ARRAY) {
				suc[zend_hash_num_elements(Z_ARR_P(op1)) != 0] = 1;
			} else {
				suc[0] = 1;
				suc[1] = 1;
			}
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

static void visit_phi(void *void_ctx, zend_ssa_phi *phi) {
	scp_ctx *ctx = (scp_ctx *) void_ctx;
	zend_ssa *ssa = ctx->ssa;
	ZEND_ASSERT(phi->ssa_var >= 0);
	if (!IS_BOT(&ctx->values[phi->ssa_var])) {
		zend_basic_block *block = &ssa->cfg.blocks[phi->block];
		int *predecessors = &ssa->cfg.predecessors[block->predecessor_offset];

		int i;
		zval result;
		MAKE_TOP(&result);
		SCP_DEBUG("Handling PHI(");
		if (phi->pi >= 0) {
			if (phi->sources[0] >= 0 && scdf_is_edge_feasible(&ctx->scdf, phi->pi, phi->block)) {
				join_phi_values(&result, &ctx->values[phi->sources[0]]);
			}
		} else {
			for (i = 0; i < block->predecessors_count; i++) {
				if (phi->sources[i] >= 0
						&& scdf_is_edge_feasible(&ctx->scdf, predecessors[i], phi->block)) {
					SCP_DEBUG("val, ");
					join_phi_values(&result, &ctx->values[phi->sources[i]]);
				} else {
					SCP_DEBUG("--, ");
				}
			}
		}
		SCP_DEBUG(")\n");

		set_value(ctx, phi->ssa_var, &result);
		zval_ptr_dtor_nogc(&result);
	}
}

/* Removes instructions inside dead blocks. We don't remove the actual blocks, as modifying the
 * CFG would be overly painful at this point. */
static void eliminate_dead_blocks(scp_ctx *ctx) {
	zend_ssa *ssa = ctx->ssa;
	int i;
	for (i = 0; i < ssa->cfg.blocks_count; i++) {
		if (!zend_bitset_in(ctx->scdf.executable_blocks, i)) {
			OPT_STAT(scp_dead_blocks)++;
			remove_block(ssa, i,
				&OPT_STAT(scp_dead_blocks_instrs), &OPT_STAT(scp_dead_blocks_phis));
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
				OPT_STAT(scp_const_operands)++;
				remove_op1_use(ssa, ssa_op);
			}
			if (try_replace_op2(ctx, i, opline, ssa_op)) {
				OPT_STAT(scp_const_operands)++;
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
 * CV-affecting instructions into CONST ASSIGNs. This basic DCE is performed for multiple reasons:
 * a) During operand replacement we eliminate FREEs. The corresponding computational instructions
 *    must be removed to avoid leaks. This way SCP can run independently of the full DCE pass.
 * b) The main DCE pass relies on type analysis to determine whether instructions have side-effects
 *    and can't be DCEd. This means that it will not be able collect all instructions rendered dead
 *    by SCP, because they may have potentially side-effecting types, but the actual values are
 *    not. As such doing DCE here will allow us to eliminate more dead code in combination.
 * c) The ordinary DCE pass cannot collect dead calls. However SCP can result in dead calls, which
 *    we need to collect. */
static void eliminate_dead_instructions(scp_ctx *ctx) {
	zend_ssa *ssa = ctx->ssa;
	zend_op_array *op_array = ctx->op_array;
	int i;

	/* We iterate the variables backwards, so we can eliminate sequences like INIT_ROPE
	 * and INIT_ARRAY. */
	for (i = ssa->vars_count - 1; i >= 0; i--) {
		zend_ssa_var *var = &ssa->vars[i];
		if (value_known(&ctx->values[i]) && var->definition >= 0) {
			zend_op *opline = &op_array->opcodes[var->definition];
			zend_ssa_op *ssa_op = &ssa->ops[var->definition];
			if (opline->opcode == ZEND_ASSIGN) {
				/* Leave assigns to DCE (due to dtor effects) */
				continue;
			}

			if (ssa_op->result_def >= 0 && ssa_op->op1_def < 0 && ssa_op->op2_def < 0
					&& !var_used(&ssa->vars[ssa_op->result_def])) {
				if (opline->opcode == ZEND_DO_ICALL) {
					/* Call instruction -> remove opcodes that are part of the call */
					zend_call_info *call = ctx->call_map[var->definition];
					int i;
					OPT_STAT(scp_dead_instrs) += 2 + call->num_args;
					OPT_STAT(scp_dead_calls)++;

					remove_result_def(ssa, ssa_op);
					remove_instr(ssa, opline, ssa_op);
					remove_instr(ssa, call->caller_init_opline,
						&ssa->ops[call->caller_init_opline - op_array->opcodes]);

					for (i = 0; i < call->num_args; i++) {
						remove_instr(ssa, call->arg_info[i].opline,
							&ssa->ops[call->arg_info[i].opline - op_array->opcodes]);
					}
				} else {
					/* Ordinary computational instruction -> remove it */
					OPT_STAT(scp_dead_instrs)++;
					remove_result_def(ssa, ssa_op);
					remove_instr(ssa, opline, ssa_op);
				}
			} else if (ssa_op->op1_def >= 0) {
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
					opline->result_type = IS_UNUSED;
				}

				/* Remove OP_DATA opcode */
				if (opline->opcode == ZEND_ASSIGN_DIM) {
					remove_instr(ssa, opline + 1, ssa_op + 1);
				}

				/* Convert to ASSIGN */
				opline->opcode = ZEND_ASSIGN;
				opline->op2_type = IS_CONST;
				opline->op2.constant = zend_optimizer_add_literal(op_array, val);
				Z_TRY_ADDREF_P(val);
			}
		}
		/*if (var->definition_phi && !var_used(var)) {
			OPT_STAT(scp_dead_phis)++;
			remove_phi(ssa, var->definition_phi);
		}*/
	}
}

void ssa_optimize_scp(ssa_opt_ctx *ssa_ctx) {
	zend_op_array *op_array = ssa_ctx->op_array;
	zend_ssa *ssa = ssa_ctx->ssa;
	int i;
	
	scp_ctx ctx;

	ctx.op_array = op_array;
	ctx.ssa = ssa;
	ctx.call_map = ssa_ctx->call_map;
	ctx.values = alloca(sizeof(zval) * ssa->vars_count);

	MAKE_TOP(&ctx.top);
	MAKE_BOT(&ctx.bot);

	i = 0;
	for (; i < op_array->last_var; ++i) {
		/* These are all undefined variables, which we have to mark BOT.
		 * Otherwise the undefined variable warning might not be preserved. */
		MAKE_BOT(&ctx.values[i]);
	}
	for (; i < ssa->vars_count; ++i) {
		MAKE_TOP(&ctx.values[i]);
	}

	ctx.scdf.handlers.visit_instr = visit_instr;
	ctx.scdf.handlers.visit_phi = visit_phi;
	ctx.scdf.handlers.get_feasible_successors = get_feasible_successors;

	scdf_init(&ctx.scdf, op_array, ssa);
	scdf_solve(&ctx.scdf, "SCCP");
	eliminate_dead_blocks(&ctx);
	replace_constant_operands(&ctx);
	eliminate_dead_instructions(&ctx);
	scdf_free(&ctx.scdf);

	for (i = 0; i < ssa->vars_count; ++i) {
		zval_ptr_dtor_nogc(&ctx.values[i]);
	}
}
