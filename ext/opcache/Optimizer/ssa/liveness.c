#include "php.h"
#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa/helpers.h"
#include "Optimizer/ssa/liveness.h"

/* This implements "Fast Liveness Checking for SSA-Form Programs" by Boissinot et al. */

#define LIVENESS_DEBUG 0
#if LIVENESS_DEBUG
# define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
static void zend_bitset_dump(zend_bitset bitset, uint32_t len, uint32_t count) {
	int i, j;
	for (i = 0; i < count; i++) {
		fprintf(stderr, "%d: ", i);
		j = 0;
		while ((j = zend_bitset_next(bitset + len * i, len, j)) >= 0) {
			fprintf(stderr, "%d ", j);
			j++;
		}
		fprintf(stderr, "\n");
	}
}
#else
# define DEBUG_PRINT(...)
#endif

#define REDUCED_REACHABLE(block) (liveness->reduced_reachable + (block) * liveness->block_set_len)
#define TARGETS(block) (liveness->targets + (block) * liveness->block_set_len)

static void compute_reduced_reachable(
		zend_optimizer_ctx *opt_ctx, ssa_liveness *liveness,
		const zend_cfg *cfg, const cfg_info *info) {
	/* Traverse CFG in postorder and build
	 * R_v = {v} union unionall_{v' in succ(v)} R_v' */
	int i;
	for (i = 0; i < cfg->blocks_count; i++) {
		uint32_t n = info->postorder[i];
		zend_basic_block *block = &cfg->blocks[n];
		zend_bitset_incl(REDUCED_REACHABLE(n), n);
		if (block->successors[0] >= 0) {
			zend_bitset_union(REDUCED_REACHABLE(n),
				REDUCED_REACHABLE(block->successors[0]), liveness->block_set_len);
			if (block->successors[1] >= 0) {
				zend_bitset_union(REDUCED_REACHABLE(n),
					REDUCED_REACHABLE(block->successors[1]), liveness->block_set_len);
			}
		}
	}
}

static void compute_targets(
		zend_optimizer_ctx *opt_ctx, ssa_liveness *liveness,
		const zend_cfg *cfg, const cfg_info *info) {
	ALLOCA_FLAG(use_heap);
	zend_bitset tmp = ZEND_BITSET_ALLOCA(liveness->block_set_len, use_heap);
	int i;

	/* Go through CFG nodes in preorder and for each v that is a backedge target:
	 * Consider backedges (source, target) where source is reduced reachable from v
	 * and target is not reduced reachable from v. Take the union of all
	 * TARGETS(target) and {v}. */
	for (i = 0; i < cfg->blocks_count; i++) {
		int n = info->preorder[i];
		if (zend_bitset_in(info->backedge_targets, n)) {
			int source = 0;
			zend_bitset_copy(tmp, REDUCED_REACHABLE(n), liveness->block_set_len);
			zend_bitset_intersection(tmp, info->backedge_sources, liveness->block_set_len);
			while ((source = zend_bitset_next(tmp, liveness->block_set_len, source)) >= 0) {
				int s;
				for (s = 0; s < 2; s++) {
					if (zend_bitset_in(info->backedges, 2 * source + s)) {
						int target = cfg->blocks[source].successors[s];
						ZEND_ASSERT(target >= 0);
						if (!zend_bitset_in(REDUCED_REACHABLE(n), target)) {
							zend_bitset_union(TARGETS(n), TARGETS(target), liveness->block_set_len);
						}
					}
				}
				source++;
			}
			zend_bitset_incl(TARGETS(n), n);
		}
	}

	/* For each backedge source compute union of backedge targets */
	i = 0;
	while ((i = zend_bitset_next(info->backedges, liveness->block_set_len, i)) >= 0) {
		int source = i >> 1;
		int target = cfg->blocks[source].successors[i & 1];
		zend_bitset_union(TARGETS(source), TARGETS(target), liveness->block_set_len);
		i++;
	}

	/* Propagate info upwards through reduced graph using postorder iteration */
	for (i = cfg->blocks_count - 1; i >= 0; i--) {
		int n = info->postorder[i];
		zend_basic_block *block = &cfg->blocks[n];
		if (block->successors[0] >= 0) {
			zend_bitset_union(TARGETS(n), TARGETS(block->successors[0]), liveness->block_set_len);
			if (block->successors[1] >= 0) {
				zend_bitset_union(TARGETS(n),
					TARGETS(block->successors[1]), liveness->block_set_len);
			}
		}
	}

	/* Add v to each T_v */
	for (i = 0; i < cfg->blocks_count; i++) {
		zend_bitset_incl(TARGETS(i), i);
	}
	free_alloca(tmp, use_heap);
}

void ssa_liveness_precompute(
		zend_optimizer_ctx *opt_ctx, ssa_liveness *liveness, zend_ssa *ssa, cfg_info *info) {
	zend_cfg *cfg = &ssa->cfg;

	liveness->ssa = ssa;
	liveness->info = info;
	liveness->block_set_len = zend_bitset_len(cfg->blocks_count);
	liveness->reduced_reachable = zend_arena_calloc(&opt_ctx->arena,
		liveness->block_set_len * sizeof(zend_ulong), cfg->blocks_count);
	liveness->targets = zend_arena_calloc(&opt_ctx->arena,
		liveness->block_set_len * sizeof(zend_ulong), cfg->blocks_count);

	compute_reduced_reachable(opt_ctx, liveness, cfg, info);
	compute_targets(opt_ctx, liveness, cfg, info);
	
#if LIVENESS_DEBUG
	int i;
	if (ssa->op_array->function_name) {
		fprintf(stderr, "Function %s:\n", ZSTR_VAL(ssa->op_array->function_name));
	}
	fprintf(stderr, "Postorder numbering:\n");
	for (i = 0; i < cfg->blocks_count; i++) {
		fprintf(stderr, "%d ", info.postorder[i]);
	}
	fprintf(stderr, "\nBackedges:\n");
	for (i = 0; i < cfg->blocks_count; i++) {
		zend_basic_block *block = &cfg->blocks[i];
		if (zend_bitset_in(info.backedges, 2 * i + 0)) {
			fprintf(stderr, "%d->%d ", i, block->successors[0]);
		}
		if (zend_bitset_in(info.backedges, 2 * i + 1)) {
			fprintf(stderr, "%d->%d ", i, block->successors[1]);
		}
	}
	fprintf(stderr, "\n");
	fprintf(stderr, "Reduced reachability:\n");
	zend_bitset_dump(liveness->reduced_reachable, liveness->block_set_len, cfg->blocks_count);
	fprintf(stderr, "Targets:\n");
	zend_bitset_dump(liveness->targets, liveness->block_set_len, cfg->blocks_count);
	fprintf(stderr, "Strictly dominated:\n");
	zend_bitset_dump(liveness->info->sdom, liveness->block_set_len, cfg->blocks_count);
#endif
}

static zend_bool phi_use_reachable(
		const ssa_liveness *liveness, const zend_ssa *ssa,
		const zend_ssa_phi *phi, int from_block, int var_num) {
	/* The i-th phi operand is used along the CFG edge from the i-th predecessor. */
	if (phi->pi >= 0) {
		if (zend_bitset_in(REDUCED_REACHABLE(from_block), phi->pi)) {
			DEBUG_PRINT("Live via predecessor %d\n", phi->pi);
			return 1;
		}
		return 0;
	} else {
		zend_basic_block *phi_block = &ssa->cfg.blocks[phi->block];
		int i, end = phi_block->predecessors_count;
		for (i = 0; i < end; i++) {
			if (phi->sources[i] == var_num) {
				int predecessor = ssa->cfg.predecessors[phi_block->predecessor_offset + i];
				if (zend_bitset_in(REDUCED_REACHABLE(from_block), predecessor)) {
					DEBUG_PRINT("Live via predecessor %d\n", predecessor);
					return 1;
				}
			}
		}
		return 0;
	}
}

/* If a pi result is used as a phi operand in the same block, the usual rule about phi uses
 * occuring along the CFG edge from the predecessor does not apply. Instead this variable is
 * simply never live. */
static inline zend_bool is_immediately_consumed_pi(const zend_ssa *ssa, const zend_ssa_var *var) {
	return var->definition_phi && var->definition_phi->pi >= 0 && var->use_chain < 0 &&
		var->phi_use_chain && var->phi_use_chain->block == var->definition_phi->block;
}

zend_bool ssa_is_live_in_at_block(const ssa_liveness *liveness, int var_num, int block) {
	zend_ssa *ssa = liveness->ssa;
	zend_ssa_var *var = &ssa->vars[var_num];
	int def_block = get_def_block(ssa, var);
	int i = 0;
	DEBUG_PRINT("Live-in query for var %d (def block %d) at block %d\n", var_num, def_block, block);
	if (is_immediately_consumed_pi(ssa, var)) {
		return 0;
	}
	while ((i = zend_bitset_next(TARGETS(block), liveness->block_set_len, i)) >= 0) {
		if (block_strictly_dominates(liveness->info, def_block, i)) {
			int use;
			zend_ssa_phi *phi;
			FOREACH_USE(var, use) {
				if (zend_bitset_in(REDUCED_REACHABLE(i), ssa->cfg.map[use])) {
					DEBUG_PRINT("Live due to use at op %d in block %d reachable from %d\n",
						use, ssa->cfg.map[use], i);
					return 1;
				}
			} FOREACH_USE_END();
			FOREACH_PHI_USE(var, phi) {
				if (phi_use_reachable(liveness, ssa, phi, i, var_num)) {
					DEBUG_PRINT("Live due to use in phi for %d in block %d"
						" reachable from %d\n", phi->ssa_var, phi->block, i);
					return 1;
				}
			} FOREACH_PHI_USE_END();
		}
		i++;
	}
	return 0;
}
static inline zend_bool ssa_is_live_at_op(
		const ssa_liveness *liveness, int var_num, int op, int live_in) {
	zend_ssa *ssa = liveness->ssa;
	zend_op_array *op_array = ssa->op_array;
	zend_ssa_var *var = &ssa->vars[var_num];
	int def_block = get_def_block(ssa, var);
	int block = ssa->cfg.map[op];
	DEBUG_PRINT("Live-%s query for var %d (def block %d) at op %d in block %d\n",
		live_in ? "in" : "out", var_num, def_block, op, block);
	if (is_immediately_consumed_pi(ssa, var)) {
		return 0;
	}
	if (block == def_block) {
		int use;
		zend_ssa_phi *phi;
		if (var->definition + live_in > op) {
			return 0;
		}
		FOREACH_USE(var, use) {
			int use_block = ssa->cfg.map[use];
			if (op_array->opcodes[use].opcode == ZEND_OP_DATA) {
				use--;
			}
			if (use_block != block || use + live_in > op) {
				DEBUG_PRINT("Live due to use at op %d in block %d\n", use, use_block);
				return 1;
			}
		} FOREACH_USE_END();
		FOREACH_PHI_USE(var, phi) {
			if (phi->pi >= 0) {
				if (phi->pi != block) {
					DEBUG_PRINT("Live due to use in phi for %d via predecessor %d\n",
						phi->ssa_var, phi->pi);
					return 1;
				}
			} else {
				zend_basic_block *phi_block = &ssa->cfg.blocks[phi->block];
				int i, end = phi_block->predecessors_count;
				for (i = 0; i < end; i++) {
					if (phi->sources[i] == var_num) {
						int predecessor = ssa->cfg.predecessors[phi_block->predecessor_offset + i];
						if (predecessor != block) {
							DEBUG_PRINT("Live due to use in phi for %d via predecessor %d\n",
								phi->ssa_var, predecessor);
							return 1;
						}
					}
				}
			}
		} FOREACH_PHI_USE_END();
		return 0;
	} else {
		int i = 0;
		while ((i = zend_bitset_next(TARGETS(block), liveness->block_set_len, i)) >= 0) {
			if (block_strictly_dominates(liveness->info, def_block, i)) {
				int use;
				zend_ssa_phi *phi;
				FOREACH_USE(var, use) {
					int use_block = ssa->cfg.map[use];
					if (op_array->opcodes[use].opcode == ZEND_OP_DATA) {
						use--;
					}
					if (use_block == block && use + live_in <= op
							&& !block_is_backedge_target(liveness->info, block)) {
						continue;
					}
					if (zend_bitset_in(REDUCED_REACHABLE(i), use_block)) {
						DEBUG_PRINT("Live due to use at op %d in block %d reachable from %d\n",
							use, use_block, i);
						return 1;
					}
				} FOREACH_USE_END();
				FOREACH_PHI_USE(var, phi) {
					if (phi_use_reachable(liveness, ssa, phi, i, var_num)) {
						DEBUG_PRINT("Live due to use in phi for %d in block %d"
							" reachable from %d\n", phi->ssa_var, phi->block, i);
						return 1;
					}
				} FOREACH_PHI_USE_END();
			}
			i++;
		}
		return 0;
	}
}
zend_bool ssa_is_live_in_at_op(const ssa_liveness *liveness, int var_num, int op) {
	return ssa_is_live_at_op(liveness, var_num, op, 1);
}
zend_bool ssa_is_live_out_at_op(const ssa_liveness *liveness, int var_num, int op) {
	return ssa_is_live_at_op(liveness, var_num, op, 0);
}

zend_bool ssa_is_live_out_at_block(const ssa_liveness *liveness, int var_num, int block) {
	return -1;
}
