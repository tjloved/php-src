#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/instructions.h"
#include "Optimizer/statistics.h"

/* This pass replaces certain instructions with type-specialized variants, e.g. ADD with
 * ADD_INT or ADD_DOUBLE. Currently this is only done for scalar types, not for arrays. */

zend_bool is_power_of_two(zend_long n) {
	return n != 0 && !(n & (n - 1));
}

static void normalize_op1_type(
		zend_op_array *op_array, zend_op *opline, uint32_t *t1, uint32_t t2) {
	if (MUST_BE(t2, MAY_BE_DOUBLE) && opline->op1_type == IS_CONST) {
		zval *op1 = CT_CONSTANT_EX(op_array, opline->op1.constant);
		if (Z_TYPE_P(op1) == IS_LONG) {
			convert_to_double(op1);
			*t1 = MAY_BE_DOUBLE;
		}
	}
}
static void normalize_op2_type(
		zend_op_array *op_array, zend_op *opline, uint32_t t1, uint32_t *t2) {
	if (MUST_BE(t1, MAY_BE_DOUBLE) && opline->op2_type == IS_CONST) {
		zval *op2 = CT_CONSTANT_EX(op_array, opline->op2.constant);
		if (Z_TYPE_P(op2) == IS_LONG) {
			convert_to_double(op2);
			*t2 = MAY_BE_DOUBLE;
		}
	}
}

void ssa_optimize_type_specialization(ssa_opt_ctx *ctx) {
	zend_op_array *op_array = ctx->op_array;
	zend_ssa *ssa = ctx->ssa;
	int i;
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		zend_ssa_op *ssa_op = &ssa->ops[i];
		uint32_t t1 = OP1_INFO();
		uint32_t t2 = OP2_INFO();

		/* Skip instructions with potentially undefined CVs */
		if ((opline->op1_type != IS_UNUSED && (t1 & MAY_BE_UNDEF))
				|| (opline->op2_type != IS_UNUSED && (t2 & MAY_BE_UNDEF))) {
			continue;
		}

		switch (opline->opcode) {
			case ZEND_QM_ASSIGN:
				if (opline->op1_type == IS_CONST && MUST_BE(t1, MAY_BE_LONG)
						&& ssa->var_info[ssa_op->result_def].use_as_double) {
					zval *op1 = CT_CONSTANT_EX(op_array, opline->op1.constant);
					convert_to_double(op1);
				}
				if (!(t1 & MAY_BE_UNDEF) && !CAN_BE(t1, MAY_BE_REFCOUNTED)) {
					opline->opcode = ZEND_MOV_UNCOUNTED;
				}
				break;
			case ZEND_ASSIGN:
				if (opline->op2_type == IS_CONST && MUST_BE(t2, MAY_BE_LONG)
						&& ssa->var_info[ssa_op->op1_def].use_as_double) {
					zval *op2 = CT_CONSTANT_EX(op_array, opline->op2.constant);
					convert_to_double(op2);
				}
				break;
			case ZEND_CONCAT:
				if (!CAN_BE(t1, MAY_BE_OBJECT) && !CAN_BE(t2, MAY_BE_OBJECT)) {
					opline->opcode = ZEND_FAST_CONCAT;
				}
				break;
			case ZEND_ADD:
				if (opline->op1_type == IS_CONST && opline->op2_type == IS_CONST) {
					break;
				}
				normalize_op1_type(op_array, opline, &t1, t2);
				normalize_op2_type(op_array, opline, t1, &t2);
				if (MUST_BE(t1, MAY_BE_LONG) && MUST_BE(t2, MAY_BE_LONG)) {
					if (MUST_BE(RES_INFO(), MAY_BE_LONG)) {
						opline->opcode = ZEND_ADD_INT_NO_OVERFLOW;
					} else {
						opline->opcode = ZEND_ADD_INT;
					}
					OPT_STAT(type_spec_arithm)++;
				} else if (MUST_BE(t1, MAY_BE_DOUBLE) && MUST_BE(t2, MAY_BE_DOUBLE)) {
					opline->opcode = ZEND_ADD_FLOAT;
					OPT_STAT(type_spec_arithm)++;
				}
				break;
			case ZEND_SUB:
				if (opline->op1_type == IS_CONST && opline->op2_type == IS_CONST) {
					break;
				}
				normalize_op1_type(op_array, opline, &t1, t2);
				normalize_op2_type(op_array, opline, t1, &t2);
				if (MUST_BE(t1, MAY_BE_LONG) && MUST_BE(t2, MAY_BE_LONG)) {
					opline->opcode = ZEND_SUB_INT;
					OPT_STAT(type_spec_arithm)++;
				} else if (MUST_BE(t1, MAY_BE_DOUBLE) && MUST_BE(t2, MAY_BE_DOUBLE)) {
					opline->opcode = ZEND_SUB_FLOAT;
					OPT_STAT(type_spec_arithm)++;
				}
				break;
			case ZEND_MOD:
				if (MUST_BE(t1, MAY_BE_LONG) && opline->op2_type == IS_CONST) {
					zval *op2 = CT_CONSTANT_EX(op_array, opline->op2.constant);
					if (Z_TYPE_P(op2) == IS_LONG && is_power_of_two(Z_LVAL_P(op2))) {
						opline->opcode = ZEND_BW_AND;
						Z_LVAL_P(op2) -= 1;
					}
				}
				break;
			case ZEND_MUL:
				if (opline->op1_type == IS_CONST && opline->op2_type == IS_CONST) {
					break;
				}
				normalize_op1_type(op_array, opline, &t1, t2);
				normalize_op2_type(op_array, opline, t1, &t2);
				if (MUST_BE(t1, MAY_BE_DOUBLE) && MUST_BE(t2, MAY_BE_DOUBLE)) {
					opline->opcode = ZEND_MUL_FLOAT;
					OPT_STAT(type_spec_arithm)++;
				}
				break;
			case ZEND_DIV:
				if (opline->op1_type == IS_CONST && opline->op2_type == IS_CONST) {
					break;
				}
				normalize_op1_type(op_array, opline, &t1, t2);
				normalize_op2_type(op_array, opline, t1, &t2);
				if (MUST_BE(t1, MAY_BE_DOUBLE) && MUST_BE(t2, MAY_BE_DOUBLE)) {
					opline->opcode = ZEND_DIV_FLOAT;
					OPT_STAT(type_spec_arithm)++;
				}
				break;
			case ZEND_IS_SMALLER:
				if (opline->op1_type == IS_CONST && opline->op2_type == IS_CONST) {
					break;
				}
				normalize_op1_type(op_array, opline, &t1, t2);
				normalize_op2_type(op_array, opline, t1, &t2);
				if (MUST_BE(t1, MAY_BE_LONG) && MUST_BE(t2, MAY_BE_LONG)) {
					opline->opcode = ZEND_IS_SMALLER_INT;;
					OPT_STAT(type_spec_arithm)++;
				} else if (MUST_BE(t1, MAY_BE_DOUBLE) && MUST_BE(t2, MAY_BE_DOUBLE)) {
					opline->opcode = ZEND_IS_SMALLER_FLOAT;
					OPT_STAT(type_spec_arithm)++;
				}
				break;
			case ZEND_IS_SMALLER_OR_EQUAL:
				if (opline->op1_type == IS_CONST && opline->op2_type == IS_CONST) {
					break;
				}
				if (MUST_BE(t1, MAY_BE_LONG) && MUST_BE(t2, MAY_BE_LONG)) {
					/* Convert to IS_SMALLER if one operand is constant */
					if (opline->op2_type == IS_CONST) {
						zval *op2 = CT_CONSTANT_EX(op_array, opline->op2.constant);
						if (Z_LVAL_P(op2) == ZEND_LONG_MAX) {
							break;
						}

						Z_LVAL_P(op2)++;
						opline->opcode = ZEND_IS_SMALLER_INT;
						OPT_STAT(type_spec_arithm)++;
					} else if (opline->op1_type == IS_CONST) {
						zval *op1 = CT_CONSTANT_EX(op_array, opline->op1.constant);
						if (Z_LVAL_P(op1) == ZEND_LONG_MIN) {
							break;
						}

						Z_LVAL_P(op1)--;
						opline->opcode = ZEND_IS_SMALLER_INT;
						OPT_STAT(type_spec_arithm)++;
					}
				}
				break;
			case ZEND_PRE_INC:
			case ZEND_PRE_DEC:
				if (!RESULT_UNUSED(opline) || opline->op1_type != IS_CV
						|| !(MUST_BE(t1, MAY_BE_LONG) || MUST_BE(t1, MAY_BE_DOUBLE))) {
					break;
				}
				if (MUST_BE(t1, MAY_BE_LONG)) {
					if (opline->opcode == ZEND_PRE_INC) {
						if (MUST_BE(ssa->var_info[ssa_op->op1_def].type, MAY_BE_LONG)) {
							opline->opcode = ZEND_ADD_INT_NO_OVERFLOW;
						} else {
							opline->opcode = ZEND_ADD_INT;
						}
					} else {
						opline->opcode = ZEND_SUB_INT;
					}
					opline->op2_type = IS_CONST;
					LITERAL_LONG(opline->op2, 1);
				} else if (MUST_BE(t1, MAY_BE_DOUBLE)) {
					zval zv;
					ZVAL_DOUBLE(&zv, 1.0);
					opline->opcode = opline->opcode == ZEND_PRE_INC
						? ZEND_ADD_FLOAT : ZEND_SUB_FLOAT;
					opline->op2_type = IS_CONST;
					opline->op2.constant = zend_optimizer_add_literal(op_array, &zv);
				}
				COPY_NODE(opline->result, opline->op1);
				ssa_op->result_def = ssa_op->op1_def;
				ssa_op->op1_def = -1;
				OPT_STAT(type_spec_arithm)++;
				break;
			case ZEND_FETCH_DIM_R:
				if (!MUST_BE(t1, MAY_BE_ARRAY) || opline->op1_type == IS_CONST) {
					break;
				}
				if (MUST_BE(t2, MAY_BE_LONG)) {
					opline->opcode = ZEND_FETCH_DIM_INT;
				}
				break;
			//case ZEND_FETCH_DIM_R:
			case ZEND_ASSIGN_DIM:
				// TODO This code only collects some statistics,
				// it does not actually specialize anything (yet)
				if (MUST_BE(t1, MAY_BE_ARRAY)) {
					OPT_STAT(ts_must_be_array)++;
					if ((t1 & MAY_BE_ARRAY_KEY_LONG) == MAY_BE_ARRAY_KEY_LONG) {
						OPT_STAT(ts_must_be_int_key)++;
						if (opline->op2_type == IS_UNUSED) {
							OPT_STAT(ts_must_be_append_int_key)++;
						} else if (MUST_BE(t2, MAY_BE_LONG)) {
							OPT_STAT(ts_must_be_matching_int_key)++;
						}
					} else if ((t1 & MAY_BE_ARRAY_KEY_STRING) == MAY_BE_ARRAY_KEY_STRING) {
						OPT_STAT(ts_must_be_string_key)++;
					}
					if (((t1 & MAY_BE_ARRAY_OF_ANY) & (MAY_BE_ARRAY_OF_STRING|MAY_BE_ARRAY_OF_ARRAY|MAY_BE_ARRAY_OF_RESOURCE|MAY_BE_ARRAY_OF_OBJECT)) == 0) {
						OPT_STAT(ts_must_be_notref_values)++;
					}
				} else {
					OPT_STAT(ts_not_must_be_array)++;
				}
				break;
		}
	}
}

