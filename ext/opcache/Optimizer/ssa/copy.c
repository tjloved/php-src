#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/liveness.h"
#include "Optimizer/statistics.h"

int try_copy_propagation(ssa_opt_ctx *ctx, zend_op *opline, zend_ssa_op *ssa_op) {
	zend_op_array *op_array = ctx->op_array;
	zend_ssa *ssa = ctx->ssa;
	zend_ssa_var *lhs_var = &ssa->vars[ssa_op->op1_def];
	zend_ssa_var *rhs_var = &ssa->vars[ssa_op->op2_use];
	int use;
	//zend_ssa_phi *phi;
	if (lhs_var->phi_use_chain || rhs_var->phi_use_chain) {
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
		if ((use_op->op1_use == use && use_op->op1_def >= 0)
				|| (use_op->op2_use == use && use_op->op2_def >= 0)
				|| (use_op->result_use == use && use_op->result_def >= 0)) {
			// TODO Actually, can't we handle this via liveness as well?
			return FAILURE;
		}
	} FOREACH_USE_END();

	/* Check if there is an assignment to the RHS at which the LHS is live */
	FOREACH_USE(rhs_var, use) {
		zend_ssa_op *use_op = &ssa->ops[use];
		if ((use_op->op1_use == use && use_op->op1_def >= 0)
				|| (use_op->op2_use == use && use_op->op2_def >= 0)
				|| (use_op->result_use == use && use_op->result_def >= 0)) {
			if (ssa_is_live_in_at_op(ctx->liveness, ssa_op->op1_def, use)) {
				return FAILURE;
			}
		}
	} FOREACH_USE_END();

	/* Rename CV operands in uses */
	FOREACH_USE(lhs_var, use) {
		zend_op *use_opline = &op_array->opcodes[use];
		zend_ssa_op *use_op = &ssa->ops[use];
		if (use_op->op1_use == use) {
			COPY_NODE(use_opline->op1, opline->op2);
		}
		if (use_op->op2_use == use) {
			COPY_NODE(use_opline->op2, opline->op2);
		}
		if (use_op->result_use == use) {
			COPY_NODE(use_opline->result, opline->op2);
		}
	} FOREACH_USE_END();

	/* Update SSA */
	rename_var_uses(ssa, ssa_op->op1_def, ssa_op->op2_use);

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

		if (opline->opcode != ZEND_ASSIGN ||
			opline->op1_type != IS_CV ||
			RETURN_VALUE_USED(opline)
		) {
			continue;
		}

		if (opline->op2_type == IS_CV) {
			if (try_copy_propagation(ctx, opline, ssa_op) == SUCCESS) {
				continue;
			}
		}

		if (ssa->var_info[ssa_op->op1_use].type & (MAY_BE_REF|MAY_BE_REFCOUNTED)) {
			continue;
		}

		if (opline->op2_type & (IS_TMP_VAR|IS_VAR)) {
			zend_ssa_var *var = &ssa->vars[ssa_op->op2_use];
			/* Check that var is result of immediately preceding instruction,
			 * used only in this assignment and not a reference */
			if (var->definition >= 0 && var->definition+1 == i
					&& var->use_chain == i && ssa_op->op2_use_chain < 0 && !var->phi_use_chain
					&& !(ssa->var_info[ssa_op->op2_use].type & MAY_BE_REF)) {
				zend_ssa_op *def_ssa_op = &ssa->ops[var->definition];
				zend_op *def_opline = &op_array->opcodes[var->definition];
				/* Check that it's an "ordinary" result */
				if (def_ssa_op->result_use < 0 && def_ssa_op->result_def == ssa_op->op2_use) {
					OPT_STAT(copy_contracted_assign)++;
					/* Move CV into result of instruction */
					COPY_NODE(def_opline->result, opline->op1);
					def_ssa_op->result_def = ssa_op->op1_def;
					ssa->vars[ssa_op->op1_def].definition = var->definition;
					ssa_op->op1_def = -1;
					/* NOP out the ASSIGN */
					remove_instr(ssa, opline, ssa_op);
					var->definition = -1;
					continue;
				}
			}
		}

		OPT_STAT(copy_qm_assign)++;
		/* Replace ASSIGN by QM_ASSIGN */
		opline->opcode = ZEND_QM_ASSIGN;
		COPY_NODE(opline->result, opline->op1);
		COPY_NODE(opline->op1, opline->op2);
		opline->op2_type = IS_UNUSED;

		/* Result now defines the var */
		ssa_op->result_def = ssa_op->op1_def;
		ssa_op->op1_def = -1;

		/* Update use chain */
		// TODO Make it a helper?
		remove_op1_use(ssa, ssa_op);
		ssa_op->op1_use = ssa_op->op2_use;
		ssa_op->op1_use_chain = ssa_op->op2_use_chain;
		ssa_op->op2_use = -1;
		ssa_op->op2_use_chain = -1;
	}
}
