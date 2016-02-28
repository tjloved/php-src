#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/statistics.h"

/* This pass simplifies control flow. At present it only does one operation (merging blocks). */

#if 0
static inline zend_bool is_nop_sled(zend_op_array *op_array, int start, int end) {
	int i;
	for (i = start; i <= end; i++) {
		if (op_array->opcodes[i].opcode != ZEND_NOP) {
			return 0;
		}
	}
	return 1;
}

static inline zend_bool block_is_adjacent(
		const zend_cfg *cfg, const zend_basic_block *block, int check) {
	return block->end + 1 == cfg->blocks[check].start;
}

static inline zend_bool is_block_empty(
		const zend_cfg *cfg, zend_op_array *op_array, zend_basic_block *block) {
	zend_op *last;
	if (!is_nop_sled(op_array, block->start, block->end - 1)) {
		return 0;
	}
	if (block->successors[1] >= 0) {
		return 1;
	}

	last = &op_array->opcodes[block->end];
	if (last->opcode == ZEND_NOP) {
		return 1;
	}
	return !block_is_adjacent(cfg, block, block->successors[0]);
}
#endif

static inline int num_predecessors(zend_cfg *cfg, zend_basic_block *block) {
	int j, count = 0, *predecessors = &cfg->predecessors[block->predecessor_offset];
	for (j = 0; j < block->predecessors_count; j++) {
		if (predecessors[j] >= 0) {
			count++;
		}
	}
	return count;
}

static inline zend_bool blocks_unreachable(zend_cfg *cfg, int start, int end) {
	int j;
	for (j = start; j <= end; j++) {
		if (cfg->blocks[j].flags & ZEND_BB_REACHABLE) {
			return 0;
		}
	}
	return 1;
}

static inline void update_block_map(zend_cfg *cfg, int start, int end, int block) {
	int j;
	for (j = start; j <= end; j++) {
		cfg->map[j] = block;
	}
}

static inline void replace_predecessor(zend_cfg *cfg, zend_basic_block *block, int from, int to) {
	int j, *predecessors = &cfg->predecessors[block->predecessor_offset];
	for (j = 0; j < block->predecessors_count; j++) {
		if (predecessors[j] == from) {
			predecessors[j] = to;
		}
	}
}

static void merge_blocks(zend_cfg *cfg, int block1_num, int block2_num) {
	zend_basic_block *block1 = &cfg->blocks[block1_num];
	zend_basic_block *block2 = &cfg->blocks[block2_num];
	int s;

	ZEND_ASSERT(block1->successors[0] == block2_num);
	ZEND_ASSERT(block1->successors[1] < 0);

	/* Move successors to first block */
	block1->successors[0] = block2->successors[0];
	block1->successors[1] = block2->successors[1];

	/* Update predecessors of successors of second block */
	for (s = 0; s < 2; s++) {
		if (block2->successors[s] >= 0) {
			replace_predecessor(
				cfg, &cfg->blocks[block2->successors[s]], block2_num, block1_num);
		}
	}

	/* First block now contains instructions of second block and
	 * all the intermediate unreachable blocks */
	update_block_map(cfg, block1->end + 1, block2->end, block1_num);
	block1->end = block2->end;

	/* Second block and all intermediate unreachable blocks are now empty */
	{
		zend_basic_block *b = block1;
		while (b++ != block2) {
			b->start = block1->end + 1;
			b->end = block1->end;
		}
	}

	/* Second block became unreachable */
	block2->flags &= ~ZEND_BB_REACHABLE;
}

void ssa_optimize_simplify_cfg(ssa_opt_ctx *ssa_ctx) {
	zend_ssa *ssa = ssa_ctx->ssa;
	zend_cfg *cfg = &ssa->cfg;
	zend_op_array *op_array = ssa_ctx->op_array;
	int i;
	for (i = 0; i < cfg->blocks_count; i++) {
		zend_basic_block *block = &cfg->blocks[i];
		if (!(block->flags & ZEND_BB_REACHABLE)) {
			continue;
		}

		/* Terminal block -- we're not interested */
		if (block->successors[0] < 0) {
			continue;
		}

		/* Merge two blocks that have only one successor / predecessor respectively and are only
		 * separated by unreachable blocks. */
		if (block->successors[0] > i && block->successors[1] < 0) {
			zend_basic_block *next = &cfg->blocks[block->successors[0]];
			zend_ssa_block *next_ssa = &ssa->blocks[block->successors[0]];
			if (num_predecessors(cfg, next) == 1
					&& blocks_unreachable(cfg, i + 1, block->successors[0] - 1)) {
				zend_op *opline = &op_array->opcodes[block->end];
				if (next_ssa->phis) {
					/* The block may contain pi statements -- unclear if we ought to just drop
					 * them and merge or leave them alone and not merge. */
					continue;
				}

				if (opline->opcode == ZEND_JMP) {
					MAKE_NOP(opline);
				}

				merge_blocks(cfg, i, block->successors[0]);
				OPT_STAT(cfg_merged_blocks)++;

				/* Give the new block another try */
				i--;
				continue;
			}
		}

		// TODO There is more to do here...
	}
}
