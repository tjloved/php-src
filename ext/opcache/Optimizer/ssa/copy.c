#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/liveness.h"
#include "Optimizer/statistics.h"

static inline zend_bool has_improper_op1_use(zend_op *opline) {
	return opline->opcode == ZEND_ASSIGN
		|| (opline->opcode == ZEND_UNSET_VAR && opline->extended_value & ZEND_QUICK_SET);
}

static int try_copy_propagation(ssa_opt_ctx *ctx, zend_op *opline, zend_ssa_op *ssa_op) {
	zend_op_array *op_array = ctx->op_array;
	zend_ssa *ssa = ctx->ssa;
	int lhs_var_num, rhs_var_num, old_lhs_var_num;
	zend_ssa_var *lhs_var, *rhs_var;
	int use;
	zend_ssa_phi *phi;

	if (opline->opcode == ZEND_ASSIGN) {
		lhs_var_num = ssa_op->op1_def;
		rhs_var_num = ssa_op->op2_use;
		old_lhs_var_num = ssa_op->op1_use;
	} else {
		lhs_var_num = ssa_op->result_def;
		rhs_var_num = ssa_op->op1_use;
		old_lhs_var_num = ssa->vars[lhs_var_num].var;
	}
	lhs_var = &ssa->vars[lhs_var_num];
	rhs_var = &ssa->vars[rhs_var_num];

	if (ssa->var_info[lhs_var_num].type & MAY_BE_REF) {
		return FAILURE;
	}

	if (lhs_var->phi_use_chain) {
		// TODO
		return FAILURE;
	}

	/* Check if LHS is written is */
	FOREACH_USE(lhs_var, use) {
		zend_op *use_opline = &op_array->opcodes[use];
		zend_ssa_op *use_op = &ssa->ops[use];
		if (use_opline->opcode == ZEND_BIND_LEXICAL) {
			// TODO allow if same name?
			return FAILURE;
		}
		if (has_improper_op1_use(use_opline)) {
			continue;
		}
		if ((use_op->op1_use == lhs_var_num && use_op->op1_def >= 0)
				|| (use_op->op2_use == lhs_var_num && use_op->op2_def >= 0)
				|| (use_op->result_use == lhs_var_num && use_op->result_def >= 0)) {
			return FAILURE;
		}
	} FOREACH_USE_END();

	/* Check if there is an assignment to the RHS at which the LHS is live */
	FOREACH_USE(rhs_var, use) {
		zend_ssa_op *use_op = &ssa->ops[use];
		if ((use_op->op1_use == rhs_var_num && use_op->op1_def >= 0)
				|| (use_op->op2_use == rhs_var_num && use_op->op2_def >= 0)
				|| (use_op->result_use == rhs_var_num && use_op->result_def >= 0)) {
			if (ssa_is_live_out_at_op(ctx->liveness, lhs_var_num, use)) {
				return FAILURE;
			}
		}
	} FOREACH_USE_END();

	/* Use in phi *might* lead to an assignment, pessimistically assume it does */
	FOREACH_PHI_USE(rhs_var, phi) {
		if (ssa_is_live_in_at_block(ctx->liveness, lhs_var_num, phi->block)) {
			return FAILURE;
		}
	} FOREACH_PHI_USE_END();

	/* Rename CV operands in uses */
	FOREACH_USE(lhs_var, use) {
		zend_op *use_opline = &op_array->opcodes[use];
		zend_ssa_op *use_op = &ssa->ops[use];
		if (use_opline->opcode == ZEND_UNSET_VAR && (use_opline->extended_value & ZEND_QUICK_SET)) {
			rename_var_uses(ssa, use_op->op1_def, old_lhs_var_num);
			remove_op1_def(ssa, use_op);
			remove_instr(ssa, use_opline, use_op);
			continue;
		}
		if (use_opline->opcode == ZEND_ASSIGN && use_op->op1_use == lhs_var_num) {
			set_op1_use(ssa, use_op, old_lhs_var_num);
			if (use_op->op2_use == lhs_var_num) {
				use_opline->op2.var = (zend_uintptr_t) ZEND_CALL_VAR_NUM(NULL, rhs_var->var);
			}
			continue;
		}
		if (use_op->op1_use == lhs_var_num) {
			use_opline->op1.var = (zend_uintptr_t) ZEND_CALL_VAR_NUM(NULL, rhs_var->var);
		}
		if (use_op->op2_use == lhs_var_num) {
			use_opline->op2.var = (zend_uintptr_t) ZEND_CALL_VAR_NUM(NULL, rhs_var->var);
		}
		if (use_op->result_use == lhs_var_num) {
			use_opline->result.var = (zend_uintptr_t) ZEND_CALL_VAR_NUM(NULL, rhs_var->var);
		}
	} FOREACH_USE_END();

	/* Remove assignment instruction */
	rename_var_uses(ssa, lhs_var_num, rhs_var_num);
	if (opline->opcode == ZEND_ASSIGN) {
		remove_op1_def(ssa, ssa_op);
	} else {
		remove_result_def(ssa, ssa_op);
	}
	remove_instr(ssa, opline, ssa_op);

	OPT_STAT(copy_propagated)++;
	return SUCCESS;
}

void ssa_optimize_copy(ssa_opt_ctx *ctx) {
	zend_op_array *op_array = ctx->op_array;
	zend_ssa *ssa = ctx->ssa;
	int i;
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		zend_ssa_op *ssa_op = &ssa->ops[i];

		if ((opline->opcode == ZEND_ASSIGN && !RETURN_VALUE_USED(opline)
				&& opline->op1_type == IS_CV && opline->op2_type == IS_CV
				&& !(ssa->var_info[ssa_op->op2_use].type & MAY_BE_UNDEF))
			|| (opline->opcode == ZEND_QM_ASSIGN
				&& opline->result_type == IS_CV && opline->op1_type == IS_CV
				&& !(ssa->var_info[ssa_op->op1_use].type & MAY_BE_UNDEF))) {
			if (try_copy_propagation(ctx, opline, ssa_op) == SUCCESS) {
				continue;
			}
		}
	}
}
