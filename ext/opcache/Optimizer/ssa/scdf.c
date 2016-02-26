#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/scdf.h"

/* This defines a generic framework for sparse conditional dataflow propagation. The algorithm is
 * based on "Sparse conditional constant propagation" by Wegman and Zadeck. We're using a
 * generalized implementation as described in chapter 8.3 of the SSA book.
 *
 * Every SSA variable is associated with an element on a finite-height lattice, those value can only
 * ever be lowered during the operation of the algorithm. If a value is lowered all instructions and
 * phis using that value need to be reconsidered (this is done by adding the variable to a
 * worklist). For phi functions the result is computed by applying the meet operation to the
 * operands. This continues until a fixed point is reached.
 *
 * The algorithm is control-flow sensitive: All blocks except the start block are initially assumed
 * to be unreachable. When considering a branch instruction, we determine the feasible successors
 * based on the current state of the variable lattice. If a new edge becomes feasible we either have
 * to mark the successor block executable and consider all instructions in it, or, if the target is
 * already executable, we only have to reconsider the phi functions (as we only consider phi
 * operands which are associated with a feasible edge).
 *
 * The generic framework requires the definition of three functions:
 * * visit_instr() should recompute the lattice values of all SSA variables defined by an
 *   instruction.
 * * visit_phi() should recompute the lattice value of the SSA variable defined by the phi. While
 *   doing this it should only consider operands for which scfg_is_edge_feasible() returns true.
 * * get_feasible_successors() should determine the feasible successors for a branch instruction.
 *   Note that this callback only needs to handle conditional branches (with two successors).
 */

#if 0
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

static void mark_edge_feasible(scdf_ctx *ctx, int from, int to, int suc_num) {
	int edge = from * 2 + suc_num;
	if (zend_bitset_in(ctx->feasible_edges, edge)) {
		/* We already handled this edge */
		return;
	}

	DEBUG_PRINT("Marking edge %d->%d (successor %d) feasible\n", from, to, suc_num);
	zend_bitset_incl(ctx->feasible_edges, edge);

	if (!zend_bitset_in(ctx->executable_blocks, to)) {
		DEBUG_PRINT("Marking block %d executable\n", to);
		zend_bitset_incl(ctx->executable_blocks, to);
		zend_bitset_incl(ctx->block_worklist, to);
	} else {
		/* Block is already executable, only a new edge became feasible.
		 * Reevaluate phi nodes to account for changed source operands. */
		zend_ssa_block *ssa_block = &ctx->ssa->blocks[to];
		zend_ssa_phi *phi;
		for (phi = ssa_block->phis; phi; phi = phi->next) {
			ctx->handlers.visit_phi(ctx, phi);
		}
	}
}

/* Returns whether there is a successor */
static inline zend_bool get_feasible_successors(
		scdf_ctx *ctx, zend_basic_block *block,
		zend_op *opline, zend_ssa_op *ssa_op, zend_bool *suc) {
	/* Terminal block without successors */
	if (block->successors[0] < 0) {
		return 0;
	}

	/* Unconditional jump */
	if (block->successors[1] < 0) {
		suc[0] = 1;
		return 1;
	}

	return ctx->handlers.get_feasible_successors(ctx, block, opline, ssa_op, suc);
}

static void handle_instr(scdf_ctx *ctx, int block_num, zend_op *opline, zend_ssa_op *ssa_op) {
	zend_basic_block *block = &ctx->ssa->cfg.blocks[block_num];
	ctx->handlers.visit_instr(ctx, opline, ssa_op);

	if (block->end == opline - ctx->op_array->opcodes) {
		zend_bool suc[2] = {0};
		if (get_feasible_successors(ctx, block, opline, ssa_op, suc)) {
			if (suc[0]) {
				mark_edge_feasible(ctx, block_num, block->successors[0], 0);
			}
			if (suc[1]) {
				mark_edge_feasible(ctx, block_num, block->successors[1], 1);
			}
		}
	}
}

void scdf_init(scdf_ctx *ctx, zend_op_array *op_array, zend_ssa *ssa) {
	zend_ulong *bitsets;

	ctx->op_array = op_array;
	ctx->ssa = ssa;

	ctx->var_worklist_len = zend_bitset_len(ssa->vars_count);
	ctx->block_worklist_len = zend_bitset_len(ssa->cfg.blocks_count);

	bitsets = safe_emalloc(
		ctx->var_worklist_len + 3 * ctx->block_worklist_len, sizeof(zend_ulong), 0);

	ctx->var_worklist = bitsets;
	ctx->block_worklist = ctx->var_worklist + ctx->var_worklist_len;
	ctx->executable_blocks = ctx->block_worklist + ctx->block_worklist_len;
	ctx->feasible_edges = ctx->executable_blocks + ctx->block_worklist_len;

	zend_bitset_clear(ctx->var_worklist, ctx->var_worklist_len);
	zend_bitset_clear(ctx->block_worklist, ctx->block_worklist_len);
	zend_bitset_clear(ctx->executable_blocks, ctx->block_worklist_len);
	zend_bitset_clear(ctx->feasible_edges, ctx->block_worklist_len * 2);

	zend_bitset_incl(ctx->block_worklist, 0);
	zend_bitset_incl(ctx->executable_blocks, 0);
}

void scdf_free(scdf_ctx *ctx) {
	efree(ctx->var_worklist);
}

void scdf_solve(scdf_ctx *ctx, const char *name) {
	zend_ssa *ssa = ctx->ssa;
	DEBUG_PRINT("Start SCDF solve (%s)\n", name);
	while (!zend_bitset_empty(ctx->var_worklist, ctx->var_worklist_len)
		|| !zend_bitset_empty(ctx->block_worklist, ctx->block_worklist_len)
	) {
		int i;
		while ((i = zend_bitset_pop_first(ctx->var_worklist, ctx->var_worklist_len)) >= 0) {
			zend_ssa_var *var = &ssa->vars[i];

			{
				int use;
				FOREACH_USE(var, use) {
					int block_num = ssa->cfg.map[use];
					if (zend_bitset_in(ctx->executable_blocks, block_num)) {
						handle_instr(ctx, block_num, &ctx->op_array->opcodes[use], &ssa->ops[use]);
					}
				} FOREACH_USE_END();
			}

			{
				zend_ssa_phi *phi;
				FOREACH_PHI_USE(var, phi) {
					if (zend_bitset_in(ctx->executable_blocks, phi->block)) {
						ctx->handlers.visit_phi(ctx, phi);
					}
				} FOREACH_PHI_USE_END();
			}
		}

		while ((i = zend_bitset_pop_first(ctx->block_worklist, ctx->block_worklist_len)) >= 0) {
			/* This block is now live. Interpret phis and instructions in it. */
			zend_basic_block *block = &ssa->cfg.blocks[i];
			zend_ssa_block *ssa_block = &ssa->blocks[i];

			DEBUG_PRINT("Pop block %d from worklist\n", i);

			{
				zend_ssa_phi *phi;
				for (phi = ssa_block->phis; phi; phi = phi->next) {
					ctx->handlers.visit_phi(ctx, phi);
				}
			}

			{
				int j;
				for (j = block->start; j <= block->end; j++) {
					handle_instr(ctx, i, &ctx->op_array->opcodes[j], &ssa->ops[j]);
				}
			}
		}
	}
}
