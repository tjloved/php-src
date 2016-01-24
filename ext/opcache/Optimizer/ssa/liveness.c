#include "php.h"
#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa/helpers.h"
#include "Optimizer/ssa/liveness.h"

/* This implements "Fast Liveness Checking for SSA-Form Programs" by Boissinot et al. */

#define REDUCED_REACHABLE(block) (liveness->reduced_reachable + (block) * liveness->block_set_len)
#define TARGETS(block) (liveness->targets + (block) * liveness->block_set_len)
#define SDOM(block) (liveness->sdom + (block) * liveness->block_set_len)

typedef struct _graphinfo {
	uint32_t *preorder;
	uint32_t *postorder;
	zend_bitset backedges;
	zend_bitset backedge_targets;
	zend_bitset backedge_sources;
} graphinfo;
typedef struct _graphinfo_state {
	zend_bitset visited;
	uint32_t *preorder;
	uint32_t *postorder;
	zend_bitset backedges;
	zend_bitset backedge_targets;
	zend_bitset backedge_sources;
} graphinfo_state;

// TODO Compute graphinfo iteratively?
static void compute_postorder_recursive(
		graphinfo_state *state, const zend_cfg *cfg, uint32_t block_num) {
	zend_basic_block *block = &cfg->blocks[block_num];
	int s;

	zend_bitset_incl(state->visited, block_num);
	*state->preorder++ = block_num;
	for (s = 0; s < 2; s++) {
		if (block->successors[s] < 0) {
			break;
		}
		if (zend_bitset_in(state->visited, block->successors[s])) {
			zend_bitset_incl(state->backedges, block_num * 2 + s);
			zend_bitset_incl(state->backedge_targets, block->successors[s]);
			zend_bitset_incl(state->backedge_sources, block_num);
		} else {
			compute_postorder_recursive(state, cfg, block->successors[s]);
		}
	}
	*state->postorder++ = block_num;
}
static void compute_graphinfo(graphinfo *info, zend_optimizer_ctx *opt_ctx, const zend_cfg *cfg) {
	ALLOCA_FLAG(use_heap);
	uint32_t block_set_len = zend_bitset_len(cfg->blocks_count);
	graphinfo_state state;
	state.visited = ZEND_BITSET_ALLOCA(block_set_len, use_heap);
	memset(state.visited, 0, block_set_len * sizeof(zend_ulong));
	info->preorder = state.preorder = zend_arena_calloc(
		&opt_ctx->arena, cfg->blocks_count, sizeof(uint32_t));
	info->postorder = state.postorder = zend_arena_calloc(
		&opt_ctx->arena, cfg->blocks_count, sizeof(uint32_t));
	info->backedges = state.backedges =  zend_arena_calloc(
		&opt_ctx->arena, zend_bitset_len(2 * cfg->blocks_count), sizeof(zend_ulong));
	info->backedge_targets = state.backedge_targets =  zend_arena_calloc(
		&opt_ctx->arena, block_set_len, sizeof(zend_ulong));
	info->backedge_sources = state.backedge_sources =  zend_arena_calloc(
		&opt_ctx->arena, block_set_len, sizeof(zend_ulong));
	compute_postorder_recursive(&state, cfg, 0);
	free_alloca(state.visited, use_heap);
}

static void compute_reduced_reachable(
		zend_optimizer_ctx *opt_ctx, ssa_liveness *liveness,
		const zend_cfg *cfg, const graphinfo *info) {
	/* Traverse CFG in reverse postorder and build
	 * R_v = {v} union unionall_{v' in succ(v)} R_v' */
	int i;
	for (i = cfg->blocks_count - 1; i >= 0; i--) {
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
		const zend_cfg *cfg, const graphinfo *info) {
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
			zend_bitset_union(tmp, info->backedge_sources, liveness->block_set_len);
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

/* Compute bitset of strictly dominated nodes */
// TODO The paper suggests totally ordering blocks by domination, so sdom is just a start + end
// offset
static void compute_sdom_recursive(ssa_liveness *liveness, const zend_cfg *cfg, int block_num) {
	int i;
	for (i = cfg->blocks[block_num].children; i >= 0; i = cfg->blocks[i].next_child) {
		compute_sdom_recursive(liveness, cfg, i);
		zend_bitset_union(SDOM(block_num), SDOM(i), liveness->block_set_len);
		zend_bitset_incl(SDOM(block_num), i);
	}
}

void ssa_liveness_precompute(zend_optimizer_ctx *opt_ctx, ssa_liveness *liveness, zend_ssa *ssa) {
	zend_cfg *cfg = &ssa->cfg;
	graphinfo info;

	liveness->ssa = ssa;
	liveness->block_set_len = zend_bitset_len(cfg->blocks_count);
	liveness->reduced_reachable = zend_arena_calloc(&opt_ctx->arena,
		liveness->block_set_len * sizeof(zend_ulong), cfg->blocks_count);
	liveness->targets = zend_arena_calloc(&opt_ctx->arena,
		liveness->block_set_len * sizeof(zend_ulong), cfg->blocks_count);
	liveness->sdom = zend_arena_calloc(&opt_ctx->arena,
		liveness->block_set_len * sizeof(zend_ulong), cfg->blocks_count);

	compute_graphinfo(&info, opt_ctx, cfg);
	compute_reduced_reachable(opt_ctx, liveness, cfg, &info);
	compute_targets(opt_ctx, liveness, cfg, &info);
	compute_sdom_recursive(liveness, cfg, 0);
	liveness->backedge_targets = info.backedge_targets;
	// TODO We're leaking graphinfo here
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

zend_bool ssa_is_live_in_at_block(const ssa_liveness *liveness, int var_num, int block) {
	zend_ssa *ssa = liveness->ssa;
	zend_ssa_var *var = &ssa->vars[var_num];
	int def_block = get_def_block(ssa, var);
	int i = 0;
	while ((i = zend_bitset_next(TARGETS(block), liveness->block_set_len, i)) >= 0) {
		if (zend_bitset_in(SDOM(def_block), i)) {
			int use;
			zend_ssa_phi *phi;
			FOREACH_USE(var, use) {
				if (zend_bitset_in(REDUCED_REACHABLE(i), ssa->cfg.map[use])) {
					return 1;
				}
			} FOREACH_USE_END();
			FOREACH_PHI_USE(var, phi) {
				if (zend_bitset_in(REDUCED_REACHABLE(i), phi->block)) {
					return 1;
				}
			} FOREACH_PHI_USE_END();
		}
		i++;
	}
	return 0;
}
zend_bool ssa_is_live_in_at_op(const ssa_liveness *liveness, int var_num, int op) {
	zend_ssa *ssa = liveness->ssa;
	zend_ssa_var *var = &ssa->vars[var_num];
	int def_block = get_def_block(ssa, var);
	int block = ssa->cfg.map[op];
	if (block == def_block) {
		int use;
		if (var->definition >= op) {
			return 0;
		}
		FOREACH_USE(var, use) {
			int use_block = ssa->cfg.map[use];
			if (use_block != block || use >= op) {
				return 1;
			}
		} FOREACH_USE_END();
		return var->phi_use_chain != NULL;
	} else {
		int i = 0;
		while ((i = zend_bitset_next(TARGETS(block), liveness->block_set_len, i)) >= 0) {
			if (zend_bitset_in(SDOM(def_block), i)) {
				int use;
				zend_ssa_phi *phi;
				FOREACH_USE(var, use) {
					int use_block = ssa->cfg.map[use];
					if (use_block == block && use < op
							&& !zend_bitset_in(liveness->backedge_targets, block)) {
						continue;
					}
					if (zend_bitset_in(REDUCED_REACHABLE(i), ssa->cfg.map[use])) {
						return 1;
					}
				} FOREACH_USE_END();
				FOREACH_PHI_USE(var, phi) {
					if (zend_bitset_in(REDUCED_REACHABLE(i), phi->block)) {
						return 1;
					}
				} FOREACH_PHI_USE_END();
			}
			i++;
		}
		return 0;
	}
}

zend_bool ssa_is_live_out_at_block(const ssa_liveness *liveness, int var_num, int block) {
	return -1;
}
