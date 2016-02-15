#ifndef _SSA_SCDF_H
#define _SSA_SCDF_H

typedef struct _scdf_ctx {
	zend_op_array *op_array;
	zend_ssa *ssa;
	zend_bitset var_worklist;
	zend_bitset block_worklist;
	zend_bitset executable_blocks;
	zend_bitset feasible_edges; /* Encoding: 2 bits per block, one for each successor */
	uint32_t var_worklist_len;
	uint32_t block_worklist_len;

	struct {
		void (*visit_instr)(void *ctx, zend_op *opline, zend_ssa_op *ssa_op);
		void (*visit_phi)(void *ctx, zend_ssa_phi *phi);
		zend_bool (*get_feasible_successors)(
			void *ctx, zend_basic_block *block,
			zend_op *opline, zend_ssa_op *ssa_op, zend_bool *suc);
	} handlers;
} scdf_ctx;

void scdf_solve(scdf_ctx *ctx, const char *name);
void scdf_init(scdf_ctx *ctx, zend_op_array *op_array, zend_ssa *ssa);
void scdf_free(scdf_ctx *ctx);

static inline void scdf_add_to_worklist(scdf_ctx *ctx, int var) {
	zend_bitset_incl(ctx->var_worklist, var);
}

static inline zend_bool scdf_is_edge_feasible(scdf_ctx *ctx, int from, int to) {
	zend_basic_block *block = &ctx->ssa->cfg.blocks[from];
	int suc;
	if (block->successors[0] == to) {
		suc = 0;
	} else if (block->successors[1] == to) {
		suc = 1;
	} else {
		ZEND_ASSERT(0);
	}
	return zend_bitset_in(ctx->feasible_edges, 2 * from + suc);
}

#endif

