#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa/helpers.h"
#include "Optimizer/ssa/instructions.h"

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

void ssa_optimize_type_specialization(zend_op_array *op_array, zend_ssa *ssa) {
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
			case ZEND_ADD:
				normalize_op1_type(op_array, opline, &t1, t2);
				normalize_op2_type(op_array, opline, t1, &t2);
				if (MUST_BE(t1, MAY_BE_LONG) && MUST_BE(t2, MAY_BE_LONG)) {
					opline->opcode = ZEND_ADD_INT;
				} else if (MUST_BE(t1, MAY_BE_DOUBLE) && MUST_BE(t2, MAY_BE_DOUBLE)) {
					opline->opcode = ZEND_ADD_DOUBLE;
				}
				break;
			case ZEND_SUB:
				normalize_op1_type(op_array, opline, &t1, t2);
				normalize_op2_type(op_array, opline, t1, &t2);
				if (MUST_BE(t1, MAY_BE_LONG) && MUST_BE(t2, MAY_BE_LONG)) {
					opline->opcode = ZEND_SUB_INT;
				} else if (MUST_BE(t1, MAY_BE_DOUBLE) && MUST_BE(t2, MAY_BE_DOUBLE)) {
					opline->opcode = ZEND_SUB_DOUBLE;
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
				normalize_op1_type(op_array, opline, &t1, t2);
				normalize_op2_type(op_array, opline, t1, &t2);
				if (MUST_BE(t1, MAY_BE_DOUBLE) && MUST_BE(t2, MAY_BE_DOUBLE)) {
					opline->opcode = ZEND_MUL_DOUBLE;
				}
				break;
			case ZEND_PRE_INC:
			case ZEND_PRE_DEC:
				if (!RESULT_UNUSED(opline) || opline->op1_type != IS_CV) {
					break;
				}
				if (MUST_BE(t1, MAY_BE_LONG)) {
					opline->opcode = opline->opcode == ZEND_PRE_INC ? ZEND_ADD_INT : ZEND_SUB_INT;
					opline->result_type = IS_CV;
					opline->result = opline->op1;
					opline->op2_type = IS_CONST;
					LITERAL_LONG(opline->op2, 1);
				} else if (MUST_BE(t1, MAY_BE_DOUBLE)) {
					zval zv;
					ZVAL_DOUBLE(&zv, 1.0);
					opline->opcode = opline->opcode == ZEND_PRE_INC
						? ZEND_ADD_DOUBLE : ZEND_SUB_DOUBLE;
					opline->result_type = IS_CV;
					opline->result = opline->op1;
					opline->op2_type = IS_CONST;
					opline->op2.constant = zend_optimizer_add_literal(op_array, &zv);
				}
				break;
		}
	}
}

