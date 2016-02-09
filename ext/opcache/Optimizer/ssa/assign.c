#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/statistics.h"

/* Assignment contraction: This optimizes sequences of the form
 *     T_i = INSTR
 *     ASSIGN CV_j, T_i
 * to
 *     CV_j = INSTR
 * if CV_j is known to contain a value that isn't reference counted (and a couple of other
 * conditions hold).
 *
 * It will also optimize other ASSIGNs into QM_ASSIGNs if the LHS isn't refcounted.
 */
void ssa_optimize_assign(ssa_opt_ctx *ctx) {
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
					OPT_STAT(assign_contracted_assign)++;
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

		OPT_STAT(assign_qm_assign)++;
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
