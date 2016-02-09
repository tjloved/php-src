#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/liveness.h"
#include "Optimizer/statistics.h"

static inline zend_bool has_improper_op1_use(zend_op *opline) {
	return opline->opcode == ZEND_ASSIGN
		|| (opline->opcode == ZEND_UNSET_VAR && opline->extended_value & ZEND_QUICK_SET);
}

static void rename_improper_use(
		zend_ssa *ssa, zend_op *use_opline, zend_ssa_op *use_op,
		int old_lhs_var_num, int lhs_var_num) {
	if (use_opline->opcode == ZEND_UNSET_VAR && (use_opline->extended_value & ZEND_QUICK_SET)) {
		rename_var_uses(ssa, use_op->op1_def, old_lhs_var_num);
		remove_op1_def(ssa, use_op);
		remove_instr(ssa, use_opline, use_op);
	} else if (use_opline->opcode == ZEND_ASSIGN && use_op->op1_use == lhs_var_num) {
		set_op1_use(ssa, use_op, old_lhs_var_num);
	}
}

static int try_propagate_cv_assignment(ssa_opt_ctx *ctx, zend_op *opline, zend_ssa_op *ssa_op) {
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

	/* Check if LHS is written to */
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
		rename_improper_use(ssa, use_opline, use_op, old_lhs_var_num, lhs_var_num);
		if (use_op->op1_use == lhs_var_num) {
			use_opline->op1.var = NUM_VAR(rhs_var->var);
		}
		if (use_op->op2_use == lhs_var_num) {
			use_opline->op2.var = NUM_VAR(rhs_var->var);
		}
		if (use_op->result_use == lhs_var_num) {
			use_opline->result.var = NUM_VAR(rhs_var->var);
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

	OPT_STAT(copy_propagated_cv)++;
	return SUCCESS;
}

static zend_bool is_ordinary_assign_use(
		zend_ssa *ssa, zend_ssa_var *var, zend_ssa_op *ssa_op, int i) {
	return var->definition >= 0 && var->definition+1 == i
			&& var->use_chain == i && ssa_op->op2_use_chain < 0 && !var->phi_use_chain
			&& !(ssa->var_info[ssa_op->op2_use].type & MAY_BE_REF);
}

static inline zend_bool can_tmpvar_op1(zend_op *opline) {
	switch (opline->opcode) {
		case ZEND_ISSET_ISEMPTY_VAR:
			/* Quick set gives special meaning to CV */
			return !(opline->extended_value & ZEND_QUICK_SET);
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
		case ZEND_ASSIGN_REF:
		case ZEND_ASSIGN_DIM:
		case ZEND_ASSIGN_OBJ:
		case ZEND_PRE_INC:
		case ZEND_PRE_DEC:
		case ZEND_POST_INC:
		case ZEND_POST_DEC:
		case ZEND_PRE_INC_OBJ:
		case ZEND_PRE_DEC_OBJ:
		case ZEND_POST_INC_OBJ:
		case ZEND_POST_DEC_OBJ:
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
		case ZEND_SEND_VAR_NO_REF:
		case ZEND_SEND_UNPACK:
		case ZEND_SEND_ARRAY:
		case ZEND_SEND_USER:
		case ZEND_RETURN_BY_REF:
		case ZEND_BIND_GLOBAL:
		case ZEND_FE_RESET_RW:
			return 0;
		default:
			return 1;
	}
}

static inline zend_bool can_tmpvar_op2(zend_op *opline) {
	switch (opline->opcode) {
		case ZEND_ASSIGN_REF:
		case ZEND_CATCH:
		case ZEND_BIND_LEXICAL:
			return 0;
	}

	return 1;
}

/* Propagates assignments of type ASSIGN CV_i, TMP_j where CV_i is used (properly) only once in
 * the same basic block and TMP_j has no dtor effect. */
void try_propagate_tmp_assignment(
		ssa_opt_ctx *ctx, zend_op *opline, zend_ssa_op *ssa_op, int op_num) {
	zend_ssa *ssa = ctx->ssa;
	zend_op_array *op_array = ctx->op_array;
	int lhs_var_num = ssa_op->op1_def;
	zend_ssa_var *rhs_var = &ssa->vars[ssa_op->op2_use];
	zend_ssa_var *lhs_var = &ssa->vars[ssa_op->op1_def];
	int use;
	zend_basic_block *block;
	zend_bool found_use = 0;

	if (!is_ordinary_assign_use(ssa, rhs_var, ssa_op, op_num)) {
		return;
	}
	if (lhs_var->phi_use_chain) {
		return;
	}
	if (ssa->var_info[ssa_op->op2_use].type & MAY_HAVE_DTOR) {
		/* Dropping the assignment might shorten the lifetime */
		return;
	}

	block = &ssa->cfg.blocks[ssa->cfg.map[op_num]];
	FOREACH_USE(lhs_var, use) {
		zend_ssa_op *use_ssa_op = &ssa->ops[use];
		zend_op *use_opline = &op_array->opcodes[use];
		if (use_ssa_op->op1_use == lhs_var_num && has_improper_op1_use(use_opline)) {
			continue;
		}
		if (found_use) {
			/* Temporary can only be used once */
			return;
		}

		if (use <= op_num || use > block->end) {
			/* Use in different block. We'd have to ensure postdomination for this (or special case
			 * the non-refcounted case, in which case we'd still be violating IR invariants) */
			return;
		}

		/* Ensure that the operand can be converted to a TMP */
		if (use_ssa_op->result_use == lhs_var_num) {
			return;
		}
		if (use_ssa_op->op1_use == lhs_var_num) {
			if (!can_tmpvar_op1(use_opline)) {
				return;
			}
			if (use_ssa_op->op2_use == lhs_var_num) {
				/* Two uses in the same instruction */
				return;
			}
		} else if (use_ssa_op->op2_use == lhs_var_num) {
			if (!can_tmpvar_op2(use_opline)) {
				return;
			}
		}

		found_use = 1;
	} FOREACH_USE_END();

	if (!found_use) {
		return;
	}

	/* Actually replace uses */
	FOREACH_USE(lhs_var, use) {
		zend_ssa_op *use_op = &ssa->ops[use];
		zend_op *use_opline = &op_array->opcodes[use];
		rename_improper_use(ssa, use_opline, use_op, ssa_op->op1_use, lhs_var_num);
		if (use_op->op1_use == lhs_var_num) {
			COPY_NODE(use_opline->op1, opline->op2);
			if (use_opline->opcode == ZEND_SEND_VAR && use_opline->op1_type == IS_TMP_VAR) {
				use_opline->opcode = ZEND_SEND_VAL;
			}
		} else if (use_op->op2_use == lhs_var_num) {
			COPY_NODE(use_opline->op2, opline->op2);
		}
	} FOREACH_USE_END();

	/* Remove assignment */
	rename_var_uses(ssa, lhs_var_num, ssa_op->op2_use);
	remove_op1_def(ssa, ssa_op);
	remove_instr(ssa, opline, ssa_op);

	OPT_STAT(copy_propagated_tmp)++;
}

void ssa_optimize_copy(ssa_opt_ctx *ctx) {
	zend_op_array *op_array = ctx->op_array;
	zend_ssa *ssa = ctx->ssa;
	int i;
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		zend_ssa_op *ssa_op = &ssa->ops[i];

		if (opline->opcode == ZEND_ASSIGN) {
			if (RETURN_VALUE_USED(opline) || opline->op1_type != IS_CV
					|| (ssa->var_info[ssa_op->op1_def].type & MAY_BE_REF)) {
				continue;
			}
			if (opline->op2_type == IS_CV
					&& !(ssa->var_info[ssa_op->op2_use].type & MAY_BE_UNDEF)) {
				try_propagate_cv_assignment(ctx, opline, ssa_op);
				continue;
			}
			if (opline->op2_type & (IS_VAR|IS_TMP_VAR)) {
				try_propagate_tmp_assignment(ctx, opline, ssa_op, i);
				continue;
			}
		} else if (opline->opcode == ZEND_QM_ASSIGN
				&& opline->result_type == IS_CV && opline->op1_type == IS_CV
				&& !(ssa->var_info[ssa_op->op1_use].type & MAY_BE_UNDEF)) {
			try_propagate_cv_assignment(ctx, opline, ssa_op);
			continue;
		}
	}
}
