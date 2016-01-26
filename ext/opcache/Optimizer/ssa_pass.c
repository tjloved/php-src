#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/zend_dump.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/liveness.h"
#include "Optimizer/statistics.h"

#define SSA_VERIFY_INTEGRITY ZEND_DEBUG

static void collect_ssa_stats(zend_op_array *op_array, zend_ssa *ssa) {
	int i;

	OPT_STAT(instrs) += op_array->last;
	OPT_STAT(cfg_blocks) += ssa->cfg.blocks_count;
	OPT_STAT(ssa_vars) += ssa->vars_count;

	for (i = 0; i < ssa->vars_count; i++) {
		zend_ssa_var_info *info = &ssa->var_info[i];
		zend_ssa_var *var = &ssa->vars[i];
		zend_bool is_cv = var->var < op_array->last_var;
		if (is_cv) {
			OPT_STAT(cv_ssa_vars)++;
			if (info->type & MAY_BE_UNDEF) {
				if ((info->type & MAY_BE_ANY) == 0) {
					OPT_STAT(cv_ssa_must_be_undef)++;
				} else {
					OPT_STAT(cv_ssa_may_be_undef)++;
				}
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
		if (var->definition_phi) {
			OPT_STAT(phis)++;
		}
	}
}

static void ssa_optimize_peephole(ssa_opt_ctx *ctx) {
	zend_op_array *op_array = ctx->op_array;
	zend_ssa *ssa = ctx->ssa;
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;
	while (opline != end) {
		uint32_t t1 = OP1_INFO();
		uint32_t t2 = OP2_INFO();
		switch (opline->opcode) {
			case ZEND_CONCAT:
				if (!CAN_BE(t1, MAY_BE_OBJECT) && !CAN_BE(t2, MAY_BE_OBJECT)) {
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
			OPT_STAT(trivial_phis)++;
		} else if (phi->pi < 0 && common_source >= 0) {
			rename_var_uses(ssa, phi->ssa_var, common_source);
			remove_phi(ssa, phi);
			OPT_STAT(trivial_phis)++;
		}
	} FOREACH_PHI_END();
}

#if SSA_VERIFY_INTEGRITY
static inline zend_bool is_in_use_chain(zend_ssa *ssa, int var, int check) {
	int use;
	FOREACH_USE(&ssa->vars[var], use) {
		if (use == check) {
			return 1;
		}
	} FOREACH_USE_END();
	return 0;
}

static inline zend_bool is_in_phi_use_chain(zend_ssa *ssa, int var, zend_ssa_phi *check) {
	zend_ssa_phi *phi;
	FOREACH_PHI_USE(&ssa->vars[var], phi) {
		if (phi == check) {
			return 1;
		}
	} FOREACH_PHI_USE_END();
	return 0;
}

static inline zend_bool is_used_by_op(zend_ssa *ssa, int op, int check) {
	zend_ssa_op *ssa_op = &ssa->ops[op];
	return (ssa_op->op1_use == check)
		|| (ssa_op->op2_use == check)
		|| (ssa_op->result_use == check);
}

static inline zend_bool is_defined_by_op(zend_ssa *ssa, int op, int check) {
	zend_ssa_op *ssa_op = &ssa->ops[op];
	return (ssa_op->op1_def == check)
		|| (ssa_op->op2_def == check)
		|| (ssa_op->result_def == check);
}

static inline zend_bool is_in_phi_sources(zend_ssa *ssa, zend_ssa_phi *phi, int check) {
	int source;
	FOREACH_PHI_SOURCE(phi, source) {
		if (source == check) {
			return 1;
		}
	} FOREACH_PHI_SOURCE_END();
	return 0;
}

#define FAIL(...) do { \
	if (status == SUCCESS) { \
		fprintf(stderr, "\nIn function %s (%s):\n", \
			op_array->function_name ? ZSTR_VAL(op_array->function_name) : "{main}", extra); \
	} \
	fprintf(stderr, __VA_ARGS__); \
	status = FAILURE; \
} while (0)

#define VARFMT "%d (%s%s)"
#define VAR(i) \
	(i), (ssa->vars[i].var < op_array->last_var ? "CV $" : "TMP"), \
	(ssa->vars[i].var < op_array->last_var ? ZSTR_VAL(op_array->vars[ssa->vars[i].var]) : "")

static int ssa_verify_integrity(zend_ssa *ssa, const char *extra) {
	zend_op_array *op_array = ssa->op_array;
	zend_ssa_phi *phi;
	int i, status = SUCCESS;
	for (i = 0; i < ssa->vars_count; i++) {
		zend_ssa_var *var = &ssa->vars[i];
		int use, c;

		if (var->definition < 0 && !var->definition_phi && i > op_array->last_var) {
			if (var->use_chain >= 0) {
				FAIL("var %d without def has op uses\n", i);
			}
			if (var->phi_use_chain) {
				FAIL("var " VARFMT " without def has phi uses\n", VAR(i));
			}
		}
		if (var->definition >= 0 && var->definition_phi) {
			FAIL("var %d has both def and def_phi\n", i);
		}
		if (var->definition >= 0) {
			if (!is_defined_by_op(ssa, var->definition, i)) {
				FAIL("var %d not defined by op %d\n", i, var->definition);
			}
		}
		if (var->definition_phi) {
			if (var->definition_phi->ssa_var != i) {
				FAIL("var %d not defined by given phi\n", i);
			}
		}

		c = 0;
		FOREACH_USE(var, use) {
			if (++c > 10000) {
				FAIL("cycle in uses of %d\n", i);
				return status;
			}
			if (!is_used_by_op(ssa, use, i)) {
				fprintf(stderr, "var %d not in uses of op %d\n", i, use);
			}
		} FOREACH_USE_END();

		c = 0;
		FOREACH_PHI_USE(var, phi) {
			if (++c > 10000) {
				FAIL("cycle in phi uses of %d\n", i);
				return status;
			}
			if (!is_in_phi_sources(ssa, phi, i)) {
				FAIL("var %d not in phi sources of %d\n", i, phi->ssa_var);
			}
		} FOREACH_PHI_USE_END();
	}
	for (i = 0; i < op_array->last; i++) {
		zend_ssa_op *ssa_op = &ssa->ops[i];
		if (ssa_op->op1_use >= 0) {
			if (!is_in_use_chain(ssa, ssa_op->op1_use, i)) {
				FAIL("op1 use of %d in %d not in use chain\n", ssa_op->op1_use, i);
			}
		}
		if (ssa_op->op2_use >= 0) {
			if (!is_in_use_chain(ssa, ssa_op->op2_use, i)) {
				FAIL("op1 use of %d in %d not in use chain\n", ssa_op->op2_use, i);
			}
		}
		if (ssa_op->result_use >= 0) {
			if (!is_in_use_chain(ssa, ssa_op->result_use, i)) {
				FAIL("result use of %d in %d not in use chain\n", ssa_op->result_use, i);
			}
		}
		if (ssa_op->op1_def >= 0) {
			if (ssa->vars[ssa_op->op1_def].definition != i) {
				FAIL("op1 def of %d in %d invalid\n", ssa_op->op1_def, i);
			}
		}
		if (ssa_op->op2_def >= 0) {
			if (ssa->vars[ssa_op->op2_def].definition != i) {
				FAIL("op2 def of %d in %d invalid\n", ssa_op->op2_def, i);
			}
		}
		if (ssa_op->result_def >= 0) {
			if (ssa->vars[ssa_op->result_def].definition != i) {
				FAIL("result def of %d in %d invalid\n", ssa_op->result_def, i);
			}
		}
	}
	FOREACH_PHI(phi) {
		int source;
		FOREACH_PHI_SOURCE(phi, source) {
			if (!is_in_phi_use_chain(ssa, source, phi)) {
				FAIL("%d not in phi use chain of %d\n", phi->ssa_var, source);
			}
		} FOREACH_PHI_SOURCE_END();
		if (ssa->vars[phi->ssa_var].definition_phi != phi) {
			FAIL("%d does not define this phi\n", phi->ssa_var);
		}
	} FOREACH_PHI_END();
	return status;
}
#endif

static zend_bool is_php_errormsg_used(zend_op_array *op_array) {
	uint32_t i;
	for (i = 0; i < op_array->last_var; ++i) {
		if (zend_string_equals_literal(op_array->vars[i], "php_errormsg")) {
			return 1;
		}
	}
	return 0;
}

static void run_pass(
		ssa_opt_ctx *ctx, void (*optimize_fn)(ssa_opt_ctx *ctx),
		const char *name, uint32_t debug_level) {
	optimize_fn(ctx);
#if SSA_VERIFY_INTEGRITY
	ssa_verify_integrity(ctx->ssa, name);
#endif

	if (ZCG(accel_directives).ssa_debug_level & debug_level) {
		zend_dump_op_array(ctx->op_array, ZEND_DUMP_SSA | ZEND_DUMP_HIDE_UNUSED_VARS,
			name, ctx->ssa);
	}
}

static void optimize_ssa_impl(zend_optimizer_ctx *ctx, zend_op_array *op_array) {
	zend_call_graph call_graph;
	zend_func_info *info;
	ssa_liveness liveness;
	ssa_opt_ctx ssa_ctx;

	/* We can't currently perform data-flow analysis for code using try/catch */
	if (op_array->last_try_catch) {
		return;
	}

	/* We're rebuilding the call graph for every op array, as op arrays may be changed and
	 * even reallocated during optimizations. */
	if (zend_build_call_graph_ex(&ctx->arena, ctx->script, 0, &call_graph, op_array) != SUCCESS) {
		return;
	}

	info = ZEND_FUNC_INFO(op_array);
	if (zend_build_cfg(&ctx->arena, op_array, 0, &info->ssa.cfg, &info->flags) != SUCCESS) {
		return;
	}

	if (info->flags & ZEND_FUNC_INDIRECT_VAR_ACCESS) {
		return;
	}

	if (is_php_errormsg_used(op_array)) {
		// TODO Move this into construction phases?
		return;
	}

	if (zend_cfg_build_predecessors(&ctx->arena, &info->ssa.cfg) != SUCCESS) {
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

	if (ZCG(accel_directives).opt_statistics) {
		collect_ssa_stats(op_array, &info->ssa);
	}

	complete_block_map(&info->ssa.cfg, op_array->last);
	remove_spurious_ssa_vars(op_array, &info->ssa);
	remove_trivial_phis(&info->ssa);
	ssa_verify_integrity(&info->ssa, "before SSA pass");

	ssa_liveness_precompute(ctx, &liveness, &info->ssa);
	ssa_ctx.opt_ctx = ctx;
	ssa_ctx.op_array = op_array;
	ssa_ctx.ssa = &info->ssa;
	ssa_ctx.liveness = &liveness;

	if (ZCG(accel_directives).ssa_debug_level & 2) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA | ZEND_DUMP_HIDE_UNUSED_VARS,
			"before ssa pass", &info->ssa);
	}

	run_pass(&ssa_ctx, ssa_optimize_scp, "after SCP", 4);
	run_pass(&ssa_ctx, ssa_optimize_dce, "after DCE", 8);
	run_pass(&ssa_ctx, ssa_optimize_copy, "after copy propagation", 16);

	//ssa_optimize_cv_to_tmp(&ssa_ctx);
	ssa_optimize_type_specialization(&ssa_ctx);
	ssa_optimize_object_specialization(&ssa_ctx);
	ssa_optimize_peephole(&ssa_ctx);
	ssa_verify_integrity(&info->ssa, "after SSA pass");

	if (ZCG(accel_directives).ssa_debug_level & 1) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA | ZEND_DUMP_HIDE_UNUSED_VARS,
			"after ssa pass", &info->ssa);
	}
}

void optimize_ssa(zend_op_array *op_array, zend_optimizer_ctx *ctx) {
	void *checkpoint = zend_arena_checkpoint(ctx->arena);
	optimize_ssa_impl(ctx, op_array);
	zend_arena_release(&ctx->arena, checkpoint);
}
