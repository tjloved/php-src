#ifndef _SSA_SCDF_H
#define _SSA_SCDF_H

typedef struct _scdf_ctx {
	void *ctx;
	const zend_op_array *op_array;
	zend_ssa *ssa;
	zend_bitset instr_worklist;
	/* Represent phi-instructions through the defining var */
	zend_bitset phi_var_worklist;
	zend_bitset block_worklist;
	zend_bitset executable_blocks;
	/* Edge encoding: 2 bits per block, one for each successor */
	zend_bitset feasible_edges;
	uint32_t instr_worklist_len;
	uint32_t phi_var_worklist_len;
	uint32_t block_worklist_len;

	struct {
		void (*visit_instr)(
			struct _scdf_ctx *scdf, void *ctx, zend_op *opline, zend_ssa_op *ssa_op);
		void (*visit_phi)(
			struct _scdf_ctx *scdf, void *ctx, zend_ssa_phi *phi);
		zend_bool (*get_feasible_successors)(
			struct _scdf_ctx *scdf, void *ctx, zend_basic_block *block,
			zend_op *opline, zend_ssa_op *ssa_op, zend_bool *suc);
	} handlers;
} scdf_ctx;

void scdf_init(scdf_ctx *scdf, const zend_op_array *op_array, zend_ssa *ssa, void *ctx);
void scdf_solve(scdf_ctx *scdf, const char *name);
void scdf_free(scdf_ctx *scdf);

static inline void scdf_add_to_worklist(scdf_ctx *scdf, int var_num) {
	zend_ssa *ssa = scdf->ssa;
	zend_ssa_var *var = &ssa->vars[var_num];
	int use;
	zend_ssa_phi *phi;
	FOREACH_USE(var, use) {
		zend_bitset_incl(scdf->instr_worklist, use);
	} FOREACH_USE_END();
	FOREACH_PHI_USE(var, phi) {
		zend_bitset_incl(scdf->phi_var_worklist, phi->ssa_var);
	} FOREACH_PHI_USE_END();
}

static inline zend_bool scdf_is_edge_feasible(scdf_ctx *scdf, int from, int to) {
	zend_basic_block *block = &scdf->ssa->cfg.blocks[from];
	int suc;
	if (block->successors[0] == to) {
		suc = 0;
	} else if (block->successors[1] == to) {
		suc = 1;
	} else {
		ZEND_ASSERT(0);
	}
	return zend_bitset_in(scdf->feasible_edges, 2 * from + suc);
}

#endif

