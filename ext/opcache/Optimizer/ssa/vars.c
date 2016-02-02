#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/statistics.h"
#include "Optimizer/ssa/liveness.h"

static int block_dominates(const ssa_liveness *liveness, int a, int b) {
	return a == b || zend_bitset_in(liveness->sdom + a * liveness->block_set_len, b);
}

static uint32_t get_def_block(const zend_ssa *ssa, const zend_ssa_var *var) {
	if (var->definition >= 0) {
		return ssa->cfg.map[var->definition];
	} else if (var->definition_phi) {
		return var->definition_phi->block;
	} else {
		/* Implicit define at start of start block */
		return 0;
	}
}

static zend_bool var_dominates(
		const zend_ssa *ssa, const ssa_liveness *liveness,
		zend_ssa_var *var_a, zend_ssa_var *var_b) {
	int block_a = get_def_block(ssa, var_a);
	int block_b = get_def_block(ssa, var_b);
	ZEND_ASSERT(var_a != var_b);
	if (block_a == block_b) {
		if (var_a->definition_phi) {
			/* Earlier phi dominates later phi -- normally it wouldn't matter and any phi on a
			 * block could dominate any other, but this is important for pi nodes. */
			if (var_b->definition_phi) {
				zend_ssa_phi *cur = ssa->blocks[block_a].phis;
				for (; cur; cur = cur->next) {
					if (cur == var_a->definition_phi) {
						return 1;
					}
					if (cur == var_b->definition_phi) {
						return 0;
					}
				}
				ZEND_ASSERT(0);
			}
			return 1;
		}
		if (var_b->definition_phi) {
			return 0;
		}
		if (var_a->definition == var_b->definition) {
			/* Very crazy case where one op defines the same variable twice -- in this case the
			 * second definition wins*/
			zend_ssa_op *ssa_op = &ssa->ops[var_a->definition];
			if (ssa_op->op1_def == var_a - ssa->vars) {
				ZEND_ASSERT(ssa_op->op2_def == var_b - ssa->vars);
				return 1;
			} else {
				ZEND_ASSERT(ssa_op->op1_def == var_b - ssa->vars);
				ZEND_ASSERT(ssa_op->op2_def == var_a - ssa->vars);
				return 0;
			}
		}
		return var_a->definition < var_b->definition;
	}
	return block_dominates(liveness, block_a, block_b);
}

static zend_bool interfere_dominating(const ssa_liveness *liveness, int a, zend_ssa_var *var_b) {
	if (var_b->definition >= 0) {
		return ssa_is_live_out_at_op(liveness, a, var_b->definition);
	} else if (var_b->definition_phi) {
		//return ssa_is_live_in_at_op(liveness, a, liveness->ssa->cfg.blocks[var_b->definition_phi->block].start);
		return ssa_is_live_in_at_block(liveness, a, var_b->definition_phi->block);
	} else {
		return 1;
	}
}

static zend_bool interfere(const zend_ssa *ssa, const ssa_liveness *liveness, int a, int b) {
	zend_ssa_var *var_a = &ssa->vars[a];
	zend_ssa_var *var_b = &ssa->vars[b];
	if (var_dominates(ssa, liveness, var_a, var_b)) {
		//fprintf(stderr, "%d dominates %d\n", a, b);
		return interfere_dominating(liveness, a, var_b);
	} else {
		//fprintf(stderr, "%d dominates %d\n", b, a);
		return interfere_dominating(liveness, b, var_a);
	}
}

static void check_interferences(ssa_opt_ctx *ctx) {
	zend_ssa *ssa = ctx->ssa;
	int last_var = ctx->op_array->last_var;
	int i, j;
	for (i = 0; i < ssa->vars_count; i++) {
		zend_ssa_var *var_i = &ssa->vars[i];
		int var = var_i->var;
		if (var >= last_var || (var_i->definition < 0 && !var_i->definition_phi)) {
			continue;
		}

		for (j = 0; j < i; j++) {
			zend_ssa_var *var_j = &ssa->vars[j];
			if (var_j->var != var || (var_j->definition < 0 && !var_j->definition_phi)) {
				continue;
			}

			if (interfere(ssa, ctx->liveness, i, j)) {
				fprintf(stderr,
					"%d and %d for var %d ($%s) interfere\n",
					i, j, var, ZSTR_VAL(ctx->op_array->vars[var]));
			}
		}
	}
}

void ssa_optimize_vars(ssa_opt_ctx *ctx) {
	check_interferences(ctx); // TODO temporarily in here...

	zend_op_array *op_array = ctx->op_array;
	int i;

	ALLOCA_FLAG(use_heap1);
	ALLOCA_FLAG(use_heap2);
	uint32_t used_cvs_len = zend_bitset_len(op_array->last_var);
	zend_bitset used_cvs = ZEND_BITSET_ALLOCA(used_cvs_len, use_heap1);
	uint32_t *cv_map = do_alloca(op_array->last_var * sizeof(uint32_t), use_heap2);
	uint32_t num_cvs, tmp_offset;

	/* Determine which CVs are used */
	zend_bitset_clear(used_cvs, used_cvs_len);
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		if (opline->op1_type == IS_CV) {
			zend_bitset_incl(used_cvs, VAR_NUM(opline->op1.var));
		}
		if (opline->op2_type == IS_CV) {
			zend_bitset_incl(used_cvs, VAR_NUM(opline->op2.var));
		}
		if (opline->result_type == IS_CV) {
			zend_bitset_incl(used_cvs, VAR_NUM(opline->result.var));
		}
	}

	num_cvs = 0;
	for (i = 0; i < op_array->last_var; i++) {
		if (zend_bitset_in(used_cvs, i)) {
			cv_map[i] = num_cvs++;
		} else {
			cv_map[i] = (uint32_t) -1;
		}
	}

	free_alloca(used_cvs, use_heap1);
	if (num_cvs == op_array->last_var) {
		free_alloca(cv_map, use_heap2);
		OPT_STAT(vars_orig_cvs) += op_array->last_var;
		return;
	}

	ZEND_ASSERT(num_cvs < op_array->last_var);
	tmp_offset = op_array->last_var - num_cvs;

	/* Update CV and TMP references in opcodes */
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		if (opline->op1_type == IS_CV) {
			opline->op1.var = NUM_VAR(cv_map[VAR_NUM(opline->op1.var)]);
		} else if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
			opline->op1.var -= sizeof(zval) * tmp_offset;
		}
		if (opline->op2_type == IS_CV) {
			opline->op2.var = NUM_VAR(cv_map[VAR_NUM(opline->op2.var)]);
		} else if (opline->op2_type & (IS_VAR|IS_TMP_VAR)) {
			opline->op2.var -= sizeof(zval) * tmp_offset;
		}
		if (opline->result_type == IS_CV) {
			opline->result.var = NUM_VAR(cv_map[VAR_NUM(opline->result.var)]);
		} else if (opline->result_type & (IS_VAR|IS_TMP_VAR)) {
			opline->result.var -= sizeof(zval) * tmp_offset;
		}
	}

	/* Update TMP references in live ranges */
	if (op_array->live_range) {
		for (i = 0; i < op_array->last_live_range; i++) {
			op_array->live_range[i].var -= sizeof(zval) * tmp_offset;
		}
	}

	/* Update CV name table */
	{
		zend_string **names = safe_emalloc(sizeof(zend_string *), num_cvs, 0);
		for (i = 0; i < op_array->last_var; i++) {
			if (cv_map[i] != (uint32_t) -1) {
				names[cv_map[i]] = op_array->vars[i];
			} else {
				zend_string_release(op_array->vars[i]);
			}
		}
		efree(op_array->vars);
		op_array->vars = names;
	}

	/* Update $this reference */
	if (op_array->this_var != (uint32_t) -1) {
		op_array->this_var = NUM_VAR(cv_map[VAR_NUM(op_array->this_var)]);
	}

	OPT_STAT(vars_orig_cvs) += op_array->last_var;
	OPT_STAT(vars_dead_cvs) += op_array->last_var - num_cvs;
	op_array->last_var = num_cvs;

	free_alloca(cv_map, use_heap2);
}
