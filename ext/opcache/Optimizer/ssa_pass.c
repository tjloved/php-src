#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/zend_dump.h"
#include "Optimizer/ssa/helpers.h"
#include "Optimizer/statistics.h"

#define CANT_BE(t, name) (!(t & MAY_BE_##name))

void ssa_optimize_scp(zend_optimizer_ctx *ctx, zend_op_array *op_array, zend_ssa *ssa);
void ssa_optimize_dce(zend_optimizer_ctx *ctx, zend_op_array *op_array, zend_ssa *ssa);
void ssa_optimize_copy(zend_optimizer_ctx *ctx, zend_op_array *op_array, zend_ssa *ssa);
void ssa_optimize_cv_to_tmp(zend_optimizer_ctx *ctx, zend_op_array *op_array, zend_ssa *ssa);
void ssa_optimize_type_specialization(
		zend_optimizer_ctx *ctx, zend_op_array *op_array, zend_ssa *ssa);
void ssa_optimize_object_specialization(
		zend_optimizer_ctx *ctx, zend_op_array *op_array, zend_ssa *ssa);

static void collect_ssa_stats(zend_op_array *op_array, zend_ssa *ssa) {
	int i;

	OPT_STAT(ssa_vars) += ssa->vars_count;

	for (i = 0; i < ssa->vars_count; i++) {
		zend_ssa_var_info *info = &ssa->var_info[i];
		zend_ssa_var *var = &ssa->vars[i];
		zend_bool is_cv = var->var < op_array->last_var;
		if (is_cv) {
			OPT_STAT(cv_ssa_vars)++;
			if (info->type & MAY_BE_UNDEF) {
				OPT_STAT(cv_ssa_may_be_undef)++;
			}
		}
		if (info->type & MAY_BE_REF) {
			OPT_STAT(ssa_may_be_ref)++;
			if (is_cv) {
				OPT_STAT(cv_ssa_may_be_ref)++;
			}
		}
		if ((info->type & MAY_BE_ANY) == MAY_BE_ANY) {
			OPT_STAT(ssa_may_be_any)++;
			if (is_cv) {
				OPT_STAT(cv_ssa_may_be_any)++;
			}
		}
		if (info->type & MAY_BE_REFCOUNTED) {
			OPT_STAT(ssa_may_be_refcounted)++;
			if (is_cv) {
				OPT_STAT(cv_ssa_may_be_refcounted)++;
			}
		}
	}
}

static void ssa_optimize_peephole(zend_optimizer_ctx *ctx, zend_op_array *op_array, zend_ssa *ssa) {
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;
	while (opline != end) {
		uint32_t t1 = OP1_INFO();
		uint32_t t2 = OP2_INFO();
		switch (opline->opcode) {
			case ZEND_CONCAT:
				if (CANT_BE(t1, OBJECT) && CANT_BE(t2, OBJECT)) {
					opline->opcode = ZEND_FAST_CONCAT;
				}
				break;
		}
		opline++;
	}
}

/* The block map only contains entries for the start and end of a block.
 * Fill it up to cover all instructions. */
static void complete_block_map(zend_cfg *cfg, uint32_t num_instr) {
	uint32_t *map = cfg->map, *end = map + num_instr;
	uint32_t block = (uint32_t) -1;
	for (; map < end; map++) {
		if (*map != (uint32_t) -1) {
			block = *map;
		} else {
			*map = block;
		}
	}
}

/* Certain ops create spurious SSA var defs, which are only of interest for
 * RC1|RCN inference. As we are not interested in it, we drop them up front. */
static void remove_spurious_ssa_vars(zend_op_array *op_array, zend_ssa *ssa) {
	int i;
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		zend_ssa_op *ssa_op = &ssa->ops[i];
		switch (opline->opcode) {
			case ZEND_ASSIGN:
				if (ssa_op->op2_def >= 0) {
					rename_var_uses(ssa, ssa_op->op2_def, ssa_op->op2_use);
					remove_op2_def(ssa, ssa_op);
				}
				break;
			case ZEND_ASSIGN_DIM:
			case ZEND_ASSIGN_OBJ:
			{
				zend_ssa_op *data_op = ssa_op + 1;
				if (data_op->op1_def >= 0) {
					rename_var_uses(ssa, data_op->op1_def, data_op->op1_use);
					remove_op1_def(ssa, data_op);
				}
				break;
			}
			case ZEND_INIT_ARRAY:
			case ZEND_ADD_ARRAY_ELEMENT:
				if (opline->extended_value & ZEND_ARRAY_ELEMENT_REF) {
					break;
				}
				/* break missing intentionally */
			case ZEND_FE_RESET_R:
				if (ssa_op->op1_def >= 0) {
					rename_var_uses(ssa, ssa_op->op1_def, ssa_op->op1_use);
					remove_op1_def(ssa, ssa_op);
				}
				break;
		}
	}
}

static int get_common_phi_source(zend_ssa *ssa, zend_ssa_phi *phi) {
	int common_source = -1;
	int source;
	FOREACH_PHI_SOURCE(phi, source) {
		if (common_source == -1) {
			common_source = source;
		} else if (common_source != source) {
			return -1;
		}
	} FOREACH_PHI_SOURCE_END();
	return common_source;
}

static void remove_trivial_phis(zend_ssa *ssa) {
	zend_ssa_phi *phi;
	FOREACH_PHI(phi) {
		zend_ssa_var *var = &ssa->vars[phi->ssa_var];
		int common_source = get_common_phi_source(ssa, phi);
		if (!var_used(var)) {
			remove_phi(ssa, phi);
		} else if (phi->pi < 0 && common_source >= 0) {
			rename_var_uses(ssa, phi->ssa_var, common_source);
			remove_phi(ssa, phi);
		}
	} FOREACH_PHI_END();
}

static void verify_use_chains(zend_ssa *ssa) {
	int i;
	for (i = 0; i < ssa->vars_count; i++) {
		zend_ssa_var *var = &ssa->vars[i];
		zend_ssa_phi *phi;
		int c = 0;
		/*if (var->definition < 0 && !var->definition_phi) {
			if (var->use_chain >= 0 || var->phi_use_chain) {
				php_printf("Var %d without def has uses", i);
			}
		}*/
		FOREACH_PHI_USE(var, phi) {
			if (++c > 10000) {
				php_printf("Cycle in phi uses of %d\n", i);
				break;
			}
		} FOREACH_PHI_USE_END();
	}
}

static zend_bool is_php_errormsg_used(zend_op_array *op_array) {
	uint32_t i;
	for (i = 0; i < op_array->last_var; ++i) {
		if (zend_string_equals_literal(op_array->vars[i], "php_errormsg")) {
			return 1;
		}
	}
	return 0;
}

static void optimize_ssa_impl(zend_optimizer_ctx *ctx, zend_op_array *op_array) {
	zend_call_graph call_graph;
	zend_func_info *info;

	/* We're rebuilding the call graph for every op array, as op arrays may be changed and
	 * even reallocated during optimizations. */
	if (zend_build_call_graph_ex(&ctx->arena, ctx->script, 0, &call_graph, op_array) != SUCCESS) {
		return;
	}

	info = ZEND_FUNC_INFO(op_array);
	if (zend_build_cfg(&ctx->arena, op_array, 0, &info->ssa.cfg, &info->flags) != SUCCESS) {
		return;
	}

	if (info->flags & ZEND_FUNC_TOO_DYNAMIC) {
		return;
	}

	if (zend_cfg_build_predecessors(&ctx->arena, &info->ssa.cfg) != SUCCESS) {
		return;
	}

	if (is_php_errormsg_used(op_array)) {
		// TODO Move this into construction phases?
		return;
	}

	if (zend_cfg_compute_dominators_tree(op_array, &info->ssa.cfg) != SUCCESS) {
		return;
	}

	if (zend_cfg_identify_loops(op_array, &info->ssa.cfg, &info->flags) != SUCCESS) {
		return;
	}

	if (zend_build_ssa(&ctx->arena, op_array, 0, &info->ssa, &info->flags) != SUCCESS) {
		return;
	}

	if (zend_ssa_compute_use_def_chains(&ctx->arena, op_array, &info->ssa) != SUCCESS) {
		return;
	}

	if (zend_ssa_find_false_dependencies(op_array, &info->ssa) != SUCCESS) {
		return;
	}

	if (zend_ssa_find_sccs(op_array, &info->ssa) != SUCCESS){
		return;
	}

	if (zend_ssa_inference(&ctx->arena, op_array, ctx->script, &info->ssa) != SUCCESS) {
		return;
	}

	collect_ssa_stats(op_array, &info->ssa);

	complete_block_map(&info->ssa.cfg, op_array->last);
	remove_spurious_ssa_vars(op_array, &info->ssa);
	remove_trivial_phis(&info->ssa);
	verify_use_chains(&info->ssa);

	if (ZCG(accel_directives).jit_debug & 2) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA | ZEND_DUMP_HIDE_UNUSED_VARS, NULL, &info->ssa);
	}

	ssa_optimize_scp(ctx, op_array, &info->ssa);
	ssa_optimize_dce(ctx, op_array, &info->ssa);
	ssa_optimize_copy(ctx, op_array, &info->ssa);
	//ssa_optimize_cv_to_tmp(ctx, op_array, &info->ssa);
	ssa_optimize_type_specialization(ctx, op_array, &info->ssa);
	ssa_optimize_object_specialization(ctx, op_array, &info->ssa);
	ssa_optimize_peephole(ctx, op_array, &info->ssa);

	if (ZCG(accel_directives).jit_debug & 1) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA | ZEND_DUMP_HIDE_UNUSED_VARS, NULL, &info->ssa);
	}
}

void optimize_ssa(zend_op_array *op_array, zend_optimizer_ctx *ctx) {
	void *checkpoint = zend_arena_checkpoint(ctx->arena);
	optimize_ssa_impl(ctx, op_array);
	zend_arena_release(&ctx->arena, checkpoint);
}
