#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/zend_dump.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/liveness.h"
#include "Optimizer/ssa/instructions.h"
#include "Optimizer/statistics.h"

#if 0
/* Verify after each pass */
#define SSA_VERIFY_INTEGRITY 2
#elif ZEND_DEBUG
/* Verify once at the end */
#define SSA_VERIFY_INTEGRITY 1
#else
#define SSA_VERIFY_INTEGRITY 0
#endif

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

		if (info->type & MAY_BE_ANY) {
			int num_types = __builtin_popcount(info->type & MAY_BE_ANY);
			if ((info->type & MAY_BE_TRUE) && (info->type & MAY_BE_FALSE)) {
				/* Count booleans as single type */
				num_types--;
			}
			OPT_STAT(type_quality) += log2(num_types);
			if (is_cv) {
				if (num_types == 1) {
					OPT_STAT(cv_ssa_one_type)++;
				} else if (num_types == 2) {
					OPT_STAT(cv_ssa_two_types)++;
				} else if (num_types == 3) {
					OPT_STAT(cv_ssa_3_types)++;
				} else if (num_types == 4) {
					OPT_STAT(cv_ssa_4_types)++;
				} else if (num_types == 5 || num_types == 6) {
					OPT_STAT(cv_ssa_56_types)++;
				} else if (num_types == 7) {
					OPT_STAT(cv_ssa_7_types)++;
				} else if (num_types == 8) {
					OPT_STAT(cv_ssa_8_types)++;
				}
			}
		} else {
			OPT_STAT(ssa_may_be_nothing)++;
		}

		if (var->definition_phi) {
			OPT_STAT(phis)++;
			if (var->definition_phi->pi >= 0) {
				OPT_STAT(pis)++;
			}
		}
	}
}

static char *get_op_type_name(zend_uchar op_type) {
	switch (op_type) {
		case IS_CONST: return "CONST";
		case IS_TMP_VAR: return "TMP";
		case IS_VAR: return "VAR";
		case IS_CV: return "CV";
		case IS_UNUSED: return "UNUSED";
		default: return "???";
	}
}

#define TRACE_USE_INFO(name) \
	(ssa_op->name##_use >= 0 ? ssa->var_info[ssa_op->name##_use].type \
	 : opline->name##_type == IS_CONST \
	   ? _const_op_type(CT_CONSTANT_EX(op_array, opline->name.constant)) : -1)
#define TRACE_DEF_INFO(name) \
	(ssa_op->name##_def >= 0 ? ssa->var_info[ssa_op->name##_def].type : -1)

static void dump_instruction_trace(const zend_op_array *op_array, const zend_ssa *ssa) {
	int i;
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		zend_ssa_op *ssa_op = &ssa->ops[i];
		fprintf(stderr, "%s %s %d %d %s %d %d %s %d %d\n",
			zend_get_opcode_name(opline->opcode),
			get_op_type_name(opline->op1_type), TRACE_USE_INFO(op1), TRACE_DEF_INFO(op1),
			get_op_type_name(opline->op2_type), TRACE_USE_INFO(op2), TRACE_DEF_INFO(op2),
			get_op_type_name(opline->result_type), TRACE_USE_INFO(result), TRACE_DEF_INFO(result)
		);
	}
}

static int get_common_phi_source(zend_ssa *ssa, zend_ssa_phi *phi) {
	int common_source = -1;
	int source;
	FOREACH_PHI_SOURCE(phi, source) {
		if (common_source == -1) {
			common_source = source;
		} else if (common_source != source && source != phi->ssa_var) {
			return -1;
		}
	} FOREACH_PHI_SOURCE_END();
	return common_source;
}

/* We assume that unreachable blocks are empty in some places -- make sure this is the case
 * even if the block pass is not used. */
static void remove_unreachable_blocks(zend_ssa *ssa) {
	int i;
	for (i = 0; i < ssa->cfg.blocks_count; i++) {
		if (!(ssa->cfg.blocks[i].flags & ZEND_BB_REACHABLE)
				&& !(ssa->cfg.blocks[i].flags & ZEND_BB_UNREACHABLE_FREE)) {
			remove_block(ssa, i, NULL, NULL);
		}
	}
}

/* Used to be important to drop phis from RC inference vars. The things it finds now indicate bugs
 * in SSA construction (result not minimal or not pruned). Currently there still exists at least
 * one case where we generate non-minimal SSA due to incorrect handling of pi statements. */
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

static inline zend_bool should_dump(const zend_op_array *op_array, int debug_level) {
	if (ZCG(accel_directives).ssa_debug_level & debug_level) {
		const char *func = ZCG(accel_directives).ssa_debug_func;
		size_t len = strlen(func);
		if (len) {
			zend_string *function_name = op_array->function_name;
			if (!function_name) {
				return 0;
			}
			if (op_array->scope) {
				zend_string *class_name = op_array->scope->name;
				int total_len = ZSTR_LEN(class_name) + strlen("::") + ZSTR_LEN(function_name);
				if (total_len != len
						|| memcmp(func, ZSTR_VAL(class_name), ZSTR_LEN(class_name))
						|| memcmp(func + ZSTR_LEN(class_name), "::", strlen("::"))
						|| memcmp(func + ZSTR_LEN(class_name) + strlen("::"),
								ZSTR_VAL(function_name), ZSTR_LEN(function_name))) {
					return 0;
				}
			} else {
				if (ZSTR_LEN(function_name) != len
						|| memcmp(ZSTR_VAL(function_name), func, len)) {
					return 0;
				}
			}
		}
		return 1;
	}
	return 0;
}

static inline void debug_dump(
		const zend_op_array *op_array, const zend_ssa *ssa, const char *name, int debug_level) {
	if (should_dump(op_array, debug_level)) {
		zend_dump_op_array(op_array, ZEND_DUMP_SSA, name, ssa);
	}
}

static inline void run_pass(
		ssa_opt_ctx *ctx, void (*optimize_fn)(ssa_opt_ctx *ctx),
		const char *name, uint32_t level) {
	if (ZCG(accel_directives).ssa_opt_level & level) {
		optimize_fn(ctx);
#if SSA_VERIFY_INTEGRITY > 1
		ssa_verify_integrity(ctx->ssa, name);
#endif
		debug_dump(ctx->op_array, ctx->ssa, name, level);
	}
}

static void optimize_ssa_impl(zend_optimizer_ctx *ctx, zend_op_array *op_array) {
	zend_func_info *info;
	cfg_info cfg_info;
	ssa_liveness liveness;
	ssa_opt_ctx ssa_ctx;
	zend_bool verify_inference = ZCG(accel_directives).opt_statistics == 3;

	/* We can't currently perform data-flow analysis for code using try/catch */
	if (op_array->last_try_catch) {
		return;
	}

	info = ZEND_FUNC_INFO(op_array);
	if (zend_build_cfg(&ctx->arena, op_array,
			ZEND_CFG_NO_ENTRY_PREDECESSORS, &info->ssa.cfg, &info->flags) != SUCCESS) {
		return;
	}

	if (info->flags & ZEND_FUNC_INDIRECT_VAR_ACCESS) {
		return;
	}

	if (zend_cfg_build_predecessors(&ctx->arena, &info->ssa.cfg) != SUCCESS) {
		return;
	}

	if (zend_cfg_compute_dominators_tree(op_array, &info->ssa.cfg) != SUCCESS) {
		return;
	}

	ssa_propagate_along_domtree(op_array, &info->ssa.cfg);

	if (zend_cfg_identify_loops(op_array, &info->ssa.cfg, &info->flags) != SUCCESS) {
		return;
	}

	if (zend_build_ssa(&ctx->arena, ctx->script, op_array,
			verify_inference ? ZEND_SSA_RC_INFERENCE : 0, &info->ssa, &info->flags) != SUCCESS) {
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

	info->call_map = zend_build_call_map(&ctx->arena, info, op_array);

	/* Should happen before inference */
	remove_unreachable_blocks(&info->ssa);

	if (verify_inference) {
		/* Do not run SCP in verify-inference mode, as SCP does not support RC inference. */
		ZCG(accel_directives).ssa_opt_level &= ~SSA_PASS_COMBINE_SCP;
	}

	if (zend_ssa_inference(&ctx->arena, op_array, ctx->script, &info->ssa) != SUCCESS) {
		return;
	}

	if (OPT_STAT_ENABLED()) {
		collect_ssa_stats(op_array, &info->ssa);
	}

	if (ZCG(accel_directives).opt_statistics == 2) {
		dump_instruction_trace(op_array, &info->ssa);
	}

	if (verify_inference) {
		ctx->optimization_level &= ~ZEND_OPTIMIZER_PASS_9;
		if (should_dump(op_array, 2)) {
			zend_dump_op_array(op_array, ZEND_DUMP_SSA, "before integrity checks", &info->ssa);
		}
		ssa_verify_inference(ctx, op_array, &info->ssa);
		if (should_dump(op_array, 1)) {
			zend_dump_op_array(op_array, 0, "with integrity checks", NULL);
		}
		return;
	}

#if SSA_VERIFY_INTEGRITY > 1
	ssa_verify_integrity(&info->ssa, "before SSA pass");
#endif

	debug_dump(op_array, &info->ssa, "before SSA pass", 2);

	compute_cfg_info(&cfg_info, ctx, &info->ssa.cfg);
	ssa_liveness_precompute(ctx, &liveness, &info->ssa, &cfg_info);
	ssa_ctx.opt_ctx = ctx;
	ssa_ctx.op_array = op_array;
	ssa_ctx.ssa = &info->ssa;
	ssa_ctx.func_info = info;
	ssa_ctx.cfg_info = &cfg_info;
	ssa_ctx.liveness = &liveness;
	ssa_ctx.reorder_dtor_effects =
		(ctx->optimization_level & ZEND_OPTIMIZER_REORDER_DTOR_EFFECTS) != 0;
	ssa_ctx.assume_no_undef = (ctx->optimization_level & ZEND_OPTIMIZER_ASSUME_NO_UNDEF) != 0;

	remove_trivial_phis(&info->ssa);

	if ((ZCG(accel_directives).ssa_opt_level & SSA_PASS_COMBINE_SCP) == 0) {
		/* Only run SCP here if it wasn't already run in parallel with inference. */
		run_pass(&ssa_ctx, ssa_optimize_scp, "after SCP", SSA_PASS_SCP);
	}

	run_pass(&ssa_ctx, ssa_optimize_dce, "after DCE", SSA_PASS_DCE);
	run_pass(&ssa_ctx,
		ssa_optimize_simplify_cfg, "after CFG simplification", SSA_PASS_SIMPLIFY_CFG);
	run_pass(&ssa_ctx, ssa_optimize_copy, "after copy propagation", SSA_PASS_COPY_PROP);
	/*run_pass(&ssa_ctx, ssa_optimize_gvn, "after GVN", 64);*/
	run_pass(&ssa_ctx, ssa_optimize_dce, "after DCE 2", SSA_PASS_DCE_2);
	run_pass(&ssa_ctx, ssa_optimize_assign, "after assignment contraction", SSA_PASS_ASSIGN_CONTR);
	run_pass(&ssa_ctx, ssa_optimize_misc, "after misc", SSA_PASS_MISC);
	run_pass(&ssa_ctx,
		ssa_optimize_type_specialization, "after type specialization", SSA_PASS_TYPE_SPEC);
	run_pass(&ssa_ctx, ssa_optimize_object_specialization, "after object spec", SSA_PASS_OBJ_SPEC);

#if SSA_VERIFY_INTEGRITY
	ssa_verify_integrity(&info->ssa, "after SSA pass");
#endif

	debug_dump(op_array, &info->ssa, "after SSA pass", 1);

	/*ssa_optimize_destroy_ssa(&ssa_ctx);*/
	ssa_optimize_compact_vars(&ssa_ctx);

	if (should_dump(op_array, 4096)) {
		/* Rebuild the CFG, so SSA destruction doesn't have to maintain it */
		if (zend_build_cfg(&ctx->arena, op_array, 0, &info->ssa.cfg, &info->flags) != SUCCESS) {
			return;
		}
		if (zend_cfg_build_predecessors(&ctx->arena, &info->ssa.cfg) != SUCCESS) {
			return;
		}
		zend_dump_op_array(op_array, ZEND_DUMP_CFG, "after SSA finalization", &info->ssa.cfg);
	}
}

void zend_optimize_ssa(zend_optimizer_ctx *ctx) {
	void *checkpoint = zend_arena_checkpoint(ctx->arena);
	zend_call_graph call_graph;
	uint32_t i;

	if (zend_build_call_graph_ex(&ctx->arena, ctx->script, 0, &call_graph, NULL) == SUCCESS) {
		for (i = 0; i < call_graph.op_arrays_count; i++) {
			void *checkpoint2 = zend_arena_checkpoint(ctx->arena);
			optimize_ssa_impl(ctx, call_graph.op_arrays[i]);
			zend_arena_release(&ctx->arena, checkpoint2);
		}
	}

	zend_arena_release(&ctx->arena, checkpoint);
}
