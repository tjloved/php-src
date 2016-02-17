#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/liveness.h"
#include "Optimizer/ssa/scdf.h"
#include "Optimizer/statistics.h"

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

static inline zend_bool var_has_proper_writes(
		const zend_ssa *ssa, const zend_op_array *op_array, const zend_ssa_var *var) {
	int use;
	int var_num = var - ssa->vars;
	FOREACH_USE(var, use) {
		zend_op *use_opline = &op_array->opcodes[use];
		zend_ssa_op *use_op = &ssa->ops[use];
		if (use_opline->opcode == ZEND_BIND_LEXICAL) {
			// TODO allow if same name?
			return 1;
		}
		if (has_improper_op1_use(use_opline)) {
			continue;
		}
		if ((use_op->op1_use == var_num && use_op->op1_def >= 0)
				|| (use_op->op2_use == var_num && use_op->op2_def >= 0)
				|| (use_op->result_use == var_num && use_op->result_def >= 0)) {
			return 1;
		}
	} FOREACH_USE_END();
	return 0;
}

static inline zend_bool var_written_while_live(
		zend_ssa *ssa, const ssa_liveness *liveness,
		const zend_ssa_var *check_var, const zend_ssa_var *live_var) {
	int check_var_num = check_var - ssa->vars;
	int live_var_num = live_var - ssa->vars;

	int use;
	zend_ssa_phi *phi;

	/* Check if there is an assignment to check_var while live_var is live */
	FOREACH_USE(check_var, use) {
		zend_ssa_op *use_op = &ssa->ops[use];
		if ((use_op->op1_use == check_var_num && use_op->op1_def >= 0)
				|| (use_op->op2_use == check_var_num && use_op->op2_def >= 0)
				|| (use_op->result_use == check_var_num && use_op->result_def >= 0)) {
			if (ssa_is_live_out_at_op(liveness, live_var_num, use)) {
				return 1;
			}
		}
	} FOREACH_USE_END();

	/* Use in phi *might* lead to an assignment, pessimistically assume it does */
	FOREACH_PHI_USE(check_var, phi) {
		if (ssa_is_live_in_at_block(liveness, live_var_num, phi->block)) {
			return 1;
		}
	} FOREACH_PHI_USE_END();

	return 0;
}

static int try_propagate_cv_assignment(ssa_opt_ctx *ctx, zend_op *opline, zend_ssa_op *ssa_op) {
	zend_op_array *op_array = ctx->op_array;
	zend_ssa *ssa = ctx->ssa;
	int lhs_var_num, rhs_var_num, old_lhs_var_num;
	zend_ssa_var *lhs_var, *rhs_var;
	int use;

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
	if (var_has_proper_writes(ssa, op_array, lhs_var)) {
		return FAILURE;
	}

	/* Check if there is an assignment to the RHS at which the LHS is live */
	if (var_written_while_live(ssa, ctx->liveness, rhs_var, lhs_var)) {
		return FAILURE;
	}

	/* Rename CV operands in uses */
	FOREACH_USE(lhs_var, use) {
		zend_op *use_opline = &op_array->opcodes[use];
		zend_ssa_op *use_op = &ssa->ops[use];
		rename_improper_use(ssa, use_opline, use_op, old_lhs_var_num, lhs_var_num);
		if (use_op->op1_use == lhs_var_num) {
			use_opline->op1.var = NUM_VAR(rhs_var->var);
			use_opline->op1_type = IS_CV;
		}
		if (use_op->op2_use == lhs_var_num) {
			use_opline->op2.var = NUM_VAR(rhs_var->var);
			use_opline->op2_type = IS_CV;
		}
		if (use_op->result_use == lhs_var_num) {
			use_opline->result.var = NUM_VAR(rhs_var->var);
			use_opline->result_type = IS_CV;
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

static inline zend_bool is_used_only_in(
		zend_ssa *ssa, zend_ssa_var *var, zend_ssa_op *ssa_op, int i) {
	return var->use_chain == i && ssa_op->op2_use_chain < 0 && !var->phi_use_chain;
}

/* Propagates assignments of type ASSIGN CV_i, TMP_j where CV_i is used (properly) only once in
 * the same basic block, the use supports TMPs and TMP_j has no dtor effect. */
void try_propagate_cv_tmp_assignment(
		ssa_opt_ctx *ctx, zend_op *opline, zend_ssa_op *ssa_op, int op_num) {
	zend_ssa *ssa = ctx->ssa;
	zend_op_array *op_array = ctx->op_array;
	int lhs_var_num = ssa_op->op1_def;
	zend_ssa_var *rhs_var = &ssa->vars[ssa_op->op2_use];
	zend_ssa_var *lhs_var = &ssa->vars[ssa_op->op1_def];
	int use;
	zend_basic_block *block;
	zend_bool found_use = 0;

	if (lhs_var->phi_use_chain) {
		return;
	}

	if (!is_used_only_in(ssa, rhs_var, ssa_op, op_num)) {
		return;
	}

	if (ssa->var_info[ssa_op->op2_use].type & MAY_BE_REF) {
		/* The assignment will deref the value. */
		return;
	}

	if (!ctx->reorder_dtor_effects && (ssa->var_info[ssa_op->op2_use].type & MAY_HAVE_DTOR)) {
		/* Dropping the assignment might shorten the RHS lifetime */
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

typedef struct _context {
	scdf_ctx scdf;
	int *copy;
} context;

#define TOP (-1)
#define BOT (-2)

static inline void set_copy(context *ctx, int var, int copy) {
	/* The result var can be BOT here if it is a reference */
	if (ctx->copy[var] != BOT && ctx->copy[var] != copy) {
		ctx->copy[var] = copy;
		scdf_add_to_worklist(&ctx->scdf, var);
	}
}

void visit_instr(void *void_ctx, zend_op *opline, zend_ssa_op *ssa_op) {
	context *ctx = (context *) void_ctx;
	/*if (opline->opcode == ZEND_ASSIGN && opline->op2_type == IS_CV && ssa_op->op1_def >= 0) {
		if (ctx->copy[ssa_op->op2_use] == BOT) {
			set_copy(ctx, ssa_op->op1_def, ssa_op->op1_def);
		} else {
			set_copy(ctx, ssa_op->op1_def, ctx->copy[ssa_op->op2_use]);
		}
		return;
	}*/

	if (ssa_op->result_def >= 0) {
		set_copy(ctx, ssa_op->result_def, ssa_op->result_def);
	}
	if (ssa_op->op1_def >= 0) {
		set_copy(ctx, ssa_op->op1_def, ssa_op->op1_def);
	}
	if (ssa_op->op2_def >= 0) {
		set_copy(ctx, ssa_op->op2_def, ssa_op->op2_def);
	}
}

void visit_phi(void *void_ctx, zend_ssa_phi *phi) {
	context *ctx = (context *) void_ctx;
	zend_ssa *ssa = ctx->scdf.ssa;
	if (ctx->copy[phi->ssa_var] == BOT) {
		return;
	}

	if (phi->pi >= 0) {
		if (phi->sources[0] >= 0 && scdf_is_edge_feasible(&ctx->scdf, phi->pi, phi->block)) {
			if (ctx->copy[phi->sources[0]] != BOT) {
				//set_copy(ctx, phi->ssa_var, ctx->copy[phi->sources[0]]);
				set_copy(ctx, phi->ssa_var, phi->sources[0]);
			}
		}
	} else {
		zend_basic_block *block = &ssa->cfg.blocks[phi->block];
		int *predecessors = &ssa->cfg.predecessors[block->predecessor_offset];
		int i;
		int result = TOP;
		for (i = 0; i < block->predecessors_count; i++) {
			if (phi->sources[i] >= 0
					&& scdf_is_edge_feasible(&ctx->scdf, predecessors[i], phi->block)) {
				int copy = ctx->copy[phi->sources[i]];
				if (result == TOP) {
					result = copy;
				} else if (result != copy) {
					result = phi->ssa_var;
				}
			}
		}
		set_copy(ctx, phi->ssa_var, result);
	}
}

static int is_cond_true(context *ctx, int var_num) {
	zend_ssa_var *var = &ctx->scdf.ssa->vars[var_num];
	zend_op *opline;
	zend_ssa_op *ssa_op;
	int copy1, copy2;
	zend_bool invert;
	if (var->definition < 0) {
		return BOT;
	}

	opline = &ctx->scdf.op_array->opcodes[var->definition];
	switch (opline->opcode) {
		case ZEND_IS_EQUAL:
		case ZEND_IS_IDENTICAL:
			invert = 0;
			break;
		case ZEND_IS_NOT_EQUAL:
		case ZEND_IS_NOT_IDENTICAL:
			invert = 1;
			break;
		default:
			return BOT;
	}

	ssa_op = &ctx->scdf.ssa->ops[var->definition];
	if (ssa_op->op1_use < 0 || ssa_op->op2_use < 0) {
		return BOT;
	}

	copy1 = ctx->copy[ssa_op->op1_use];
	copy2 = ctx->copy[ssa_op->op2_use];
	if (copy1 == BOT || copy2 == BOT) {
		return BOT;
	}
	if (copy1 == TOP || copy2 == TOP) {
		return TOP;
	}
	if (copy1 != copy2) {
		return BOT;
	}

	return !invert;
}

/* Unconditional constant propagation */
zend_bool get_feasible_successors(
		void *void_ctx, zend_basic_block *block,
		zend_op *opline, zend_ssa_op *ssa_op, zend_bool *suc) {
	context *ctx = (context *) void_ctx;
	int is_true;

	/* We can't determine the branch target at compile-time for these */
	switch (opline->opcode) {
		case ZEND_ASSERT_CHECK:
		case ZEND_CATCH:
		case ZEND_DECLARE_ANON_CLASS:
		case ZEND_DECLARE_ANON_INHERITED_CLASS:
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
		case ZEND_NEW:
		case ZEND_COALESCE:
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
			suc[0] = 1;
			suc[1] = 1;
			return 1;
	}

	/* Constant op1 -- shouldn't exist at this point, don't bother */
	if (ssa_op->op1_use < 0) {
		suc[0] = 1;
		suc[1] = 1;
		return 1;
	}

	/* We can't statically determine the condition */
	is_true = is_cond_true(ctx, ssa_op->op1_use);
	if (is_true == BOT) {
		suc[0] = 1;
		suc[1] = 1;
		return 1;
	}
	if (is_true == TOP) {
		return 0;
	}

	switch (opline->opcode) {
		case ZEND_JMPZ:
		case ZEND_JMPZNZ:
		case ZEND_JMPZ_EX:
			suc[is_true] = 1;
			break;
		case ZEND_JMPNZ:
		case ZEND_JMPNZ_EX:
		case ZEND_JMP_SET:
			suc[!is_true] = 1;
			break;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
	return 1;
}

void scdf_copy_propagation(ssa_opt_ctx *ssa_ctx) {
	zend_ssa *ssa = ssa_ctx->ssa;
	zend_op_array *op_array = ssa_ctx->op_array;
	int i;
	context ctx;

	ctx.scdf.handlers.visit_instr = visit_instr;
	ctx.scdf.handlers.visit_phi = visit_phi;
	ctx.scdf.handlers.get_feasible_successors = get_feasible_successors;

	ctx.copy = alloca(sizeof(int) * ssa->vars_count);
	for (i = 0; i < op_array->last_var; i++) {
		ctx.copy[i] = BOT;
	}
	for (; i < ssa->vars_count; i++) {
		if (ssa->var_info[i].type & MAY_BE_REF) {
			ctx.copy[i] = BOT;
		} else {
			ctx.copy[i] = TOP;
		}
	}

	scdf_init(&ctx.scdf, op_array, ssa);
	scdf_solve(&ctx.scdf, "copy propagation");

	for (i = 0; i < ssa->vars_count; i++) {
		if (ctx.copy[i] != TOP && ctx.copy[i] != BOT && ctx.copy[i] != i) {
			if (ssa->vars[i].definition_phi && ssa->vars[i].definition_phi->pi < 0) {
				/*rename_var_uses(ssa, i, ctx.copy[i]);
				remove_phi(ssa, ssa->vars[i].definition_phi);*/
				//OPT_STAT(tmp)++;
			}
		}
	}

	for (i = 0; i < ssa->cfg.blocks_count; i++) {
		if (!(ssa->cfg.blocks[i].flags & ZEND_BB_REACHABLE)) {
			continue;
		}
		if (!zend_bitset_in(ctx.scdf.executable_blocks, i)) {
			remove_block(ssa, i, &OPT_STAT(tmp), &OPT_STAT(tmp));
			//OPT_STAT(tmp)++;
		}
	}

	scdf_free(&ctx.scdf);
}

void ssa_optimize_copy(ssa_opt_ctx *ctx) {
	zend_op_array *op_array = ctx->op_array;
	zend_ssa *ssa = ctx->ssa;
	int i;
	scdf_copy_propagation(ctx);
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		zend_ssa_op *ssa_op = &ssa->ops[i];

		if (opline->opcode == ZEND_ASSIGN) {
			if (RETURN_VALUE_USED(opline) || opline->op1_type != IS_CV
					|| (ssa->var_info[ssa_op->op1_use].type & MAY_BE_REF)) {
				continue;
			}
			if (!ctx->reorder_dtor_effects
					&& (ssa->var_info[ssa_op->op1_use].type & MAY_HAVE_DTOR)) {
				/* Removing the assignment may shorten LHS lifetime */
				continue;
			}
			if (opline->op2_type == IS_CV
					&& !(ssa->var_info[ssa_op->op2_use].type & MAY_BE_UNDEF)) {
				try_propagate_cv_assignment(ctx, opline, ssa_op);
				continue;
			}
			if (opline->op2_type & (IS_VAR|IS_TMP_VAR)) {
				try_propagate_cv_tmp_assignment(ctx, opline, ssa_op, i);
				continue;
			}
		} else if (opline->opcode == ZEND_QM_ASSIGN && opline->op1_type == IS_CV
				&& !(ssa->var_info[ssa_op->op1_use].type & MAY_BE_UNDEF)) {
			if (opline->result_type == IS_CV) {
				/* Can no longer happen, currently */
				ZEND_ASSERT(0);
				try_propagate_cv_assignment(ctx, opline, ssa_op);
				continue;
			}
			if (opline->result_type & (IS_VAR|IS_TMP_VAR)) {
				try_propagate_cv_assignment(ctx, opline, ssa_op);
				continue;
			}
		}
	}
}
