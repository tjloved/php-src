#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa/helpers.h"
#include "Optimizer/ssa/cfg_info.h"

typedef struct context {
	zend_bitset active;
	zend_bitset finished;
	uint32_t *preorder;
	uint32_t *postorder;
	zend_bitset backedges;
	zend_bitset backedge_targets;
	zend_bitset backedge_sources;
} context;

// TODO Compute cfg info iteratively?
static void compute_cfg_info_recursive(
		context *state, const zend_cfg *cfg, uint32_t block_num) {
	zend_basic_block *block = &cfg->blocks[block_num];
	int s;

	zend_bitset_incl(state->active, block_num);
	*state->preorder++ = block_num;
	for (s = 0; s < 2; s++) {
		if (block->successors[s] < 0) {
			break;
		}
		if (zend_bitset_in(state->active, block->successors[s])) {
			/* Backedge detected */
			zend_bitset_incl(state->backedges, block_num * 2 + s);
			zend_bitset_incl(state->backedge_targets, block->successors[s]);
			zend_bitset_incl(state->backedge_sources, block_num);
			continue;
		}
		if (!zend_bitset_in(state->finished, block->successors[s])) {
			compute_cfg_info_recursive(state, cfg, block->successors[s]);
		}
	}
	*state->postorder++ = block_num;
	zend_bitset_excl(state->active, block_num);
	zend_bitset_incl(state->finished, block_num);
}

void compute_cfg_info(cfg_info *info, zend_optimizer_ctx *opt_ctx, const zend_cfg *cfg) {
	ALLOCA_FLAG(use_heap_active);
	ALLOCA_FLAG(use_heap_finished);
	uint32_t block_set_len = zend_bitset_len(cfg->blocks_count);
	context state;
	state.active = ZEND_BITSET_ALLOCA(block_set_len, use_heap_active);
	state.finished = ZEND_BITSET_ALLOCA(block_set_len, use_heap_finished);
	zend_bitset_clear(state.active, block_set_len);
	zend_bitset_clear(state.finished, block_set_len);

	info->preorder = state.preorder = zend_arena_calloc(
		&opt_ctx->arena, cfg->blocks_count, sizeof(uint32_t));
	info->postorder = state.postorder = zend_arena_calloc(
		&opt_ctx->arena, cfg->blocks_count, sizeof(uint32_t));
	info->backedges = state.backedges = zend_arena_calloc(
		&opt_ctx->arena, zend_bitset_len(2 * cfg->blocks_count), sizeof(zend_ulong));
	info->backedge_targets = state.backedge_targets = zend_arena_calloc(
		&opt_ctx->arena, block_set_len, sizeof(zend_ulong));
	info->backedge_sources = state.backedge_sources =  zend_arena_calloc(
		&opt_ctx->arena, block_set_len, sizeof(zend_ulong));

	compute_cfg_info_recursive(&state, cfg, 0);

	free_alloca(state.active, use_heap_active);
	free_alloca(state.finished, use_heap_finished);
}
