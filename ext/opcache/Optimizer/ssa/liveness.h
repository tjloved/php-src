#ifndef _LIVENESS_H
#define _LIVENESS_H

typedef struct _ssa_liveness {
	zend_ssa *ssa;
	uint32_t block_set_len;
	zend_bitset reduced_reachable;
	zend_bitset targets;
	zend_bitset sdom;
	zend_bitset backedge_targets;
} ssa_liveness;

void ssa_liveness_precompute(zend_optimizer_ctx *opt_ctx, ssa_liveness *liveness, zend_ssa *ssa);
zend_bool ssa_is_live_in_at_block(const ssa_liveness *liveness, int var_num, int block);
zend_bool ssa_is_live_out_at_op(const ssa_liveness *liveness, int var_num, int op);

#endif

