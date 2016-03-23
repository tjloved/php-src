#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/instructions.h"
#include "Optimizer/statistics.h"

static inline zend_bool is_minus_one(zval *op) {
	if (Z_TYPE_P(op) == IS_DOUBLE) {
		return Z_DVAL_P(op) == -1.0;
	} else if (Z_TYPE_P(op) == IS_LONG) {
		return Z_LVAL_P(op) == -1;
	} else {
		return 0;
	}
}

static inline zend_bool is_used_only_in(
		const zend_ssa *ssa, const zend_ssa_var *var, const zend_ssa_op *ssa_op) {
	if (var->phi_use_chain || var->use_chain != ssa_op - ssa->ops) {
		return 0;
	}
	if (ssa_op->result_use == var - ssa->vars) {
		return ssa_op->res_use_chain < 0;
	}
	if (ssa_op->op1_use == var - ssa->vars) {
		return ssa_op->op1_use_chain < 0;
	}
	if (ssa_op->op2_use == var - ssa->vars) {
		return ssa_op->op2_use_chain < 0;
	}
	ZEND_ASSERT(0);
}

static void move_result_def(
		zend_ssa *ssa, zend_op *old_opline, zend_ssa_op *old_op,
		zend_op *new_opline, zend_ssa_op *new_op) {
	/* Remove existing result def of new instruction */
	remove_uses_of_var(ssa, new_op->result_def);
	remove_result_def(ssa, new_op);

	/* Move def from old to new instruction */
	COPY_NODE(new_opline->result, old_opline->result);
	new_op->result_def = old_op->result_def;
	old_op->result_def = -1;
	ssa->vars[new_op->result_def].definition = new_op - ssa->ops;
}

/* Reassociates floating-point operation -(C/x) to (-C)/x, as well as variations with the constant
 * in the denominator and multiplication operations. This transformation is allowed, because PHP
 * uses the floating point round-to-nearest mode, with no way to change it. This transformation
 * would not be valid in round-to-zero mode, because it is sign-dependent. */
static void reassociate_mul_minus_one(
		zend_ssa *ssa, zend_op_array *op_array,
		zend_op *opline, zend_ssa_op *ssa_op, int var_num) {
	zend_ssa_var *var = &ssa->vars[var_num];
	zend_op *def_opline;
	if (!MUST_BE(ssa->var_info[var_num].type, MAY_BE_DOUBLE)
			|| var->definition < 0
			|| !is_used_only_in(ssa, var, ssa_op)) {
		return;
	}

	def_opline = &op_array->opcodes[var->definition];
	if (def_opline->opcode == ZEND_MUL || def_opline->opcode == ZEND_DIV) {
		zend_ssa_op *def_op = &ssa->ops[var->definition];
		zval *op;
		/* Check if we have a constant double operand - if so, move the negation there */
		if (def_opline->op1_type == IS_CONST) {
			op = &ZEND_OP1_LITERAL(def_opline);
		} else if (def_opline->op2_type == IS_CONST) {
			op = &ZEND_OP2_LITERAL(def_opline);
		} else {
			return;
		}
		if (Z_TYPE_P(op) != IS_DOUBLE) {
			return;
		}
		Z_DVAL_P(op) *= -1.0;

		/* Move the TMP def and remove the MUL -1 instruction */
		move_result_def(ssa, opline, ssa_op, def_opline, def_op);
		remove_instr(ssa, opline, ssa_op);
	}
}

/* Removes a T1 = QM_ASSIGN T0 by replacing the def-point of T0 by T1, if T0 is only used in this
 * QM_ASSIGN. This is something like a "reverse copy propagation". It is useful if T1 is used in a
 * phi, because this is not handled by ordinarly copy propagation right now. */
static void reverse_propagate_assign(
		zend_ssa *ssa, zend_op_array *op_array, zend_op *opline, zend_ssa_op *ssa_op) {
	zend_ssa_var *var = &ssa->vars[ssa_op->op1_use];
	zend_ssa_op *def_op;
	if ((ssa->var_info[ssa_op->op1_use].type & MAY_BE_REF)
			|| var->definition < 0
			|| !is_used_only_in(ssa, var, ssa_op)) {
		return;
	}

	def_op = &ssa->ops[var->definition];
	if (def_op->result_def != ssa_op->op1_use) {
		return;
	}

	move_result_def(ssa, opline, ssa_op, &op_array->opcodes[var->definition], def_op);
	remove_instr(ssa, opline, ssa_op);
}

void ssa_optimize_misc(ssa_opt_ctx *ctx) {
	zend_op_array *op_array = ctx->op_array;
	zend_ssa *ssa = ctx->ssa;
	zend_ssa_phi *phi;
	int i;

	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		zend_ssa_op *ssa_op = &ssa->ops[i];

		if (opline->opcode == ZEND_MUL) {
			if (opline->op1_type == IS_CONST && opline->op2_type == IS_TMP_VAR
					&& is_minus_one(&ZEND_OP1_LITERAL(opline))) {
				reassociate_mul_minus_one(ssa, op_array, opline, ssa_op, ssa_op->op2_use);
			}
			if (opline->op2_type == IS_CONST && opline->op1_type == IS_TMP_VAR
					&& is_minus_one(&ZEND_OP2_LITERAL(opline))) {
				reassociate_mul_minus_one(ssa, op_array, opline, ssa_op, ssa_op->op1_use);
			}
		} else if (opline->opcode == ZEND_QM_ASSIGN) {
			if (opline->result_type == IS_TMP_VAR && opline->op1_type == IS_TMP_VAR) {
				reverse_propagate_assign(ssa, op_array, opline, ssa_op);
			}
		}
	}

	FOREACH_PHI(phi) {
		zend_ssa_var *var = &ssa->vars[phi->ssa_var];
		if (phi->var < op_array->last_var || var->definition < 0) {
			/* Only interested in temporaries here */
			continue;
		}
	} FOREACH_PHI_END();
}
