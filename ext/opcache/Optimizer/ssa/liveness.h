#ifndef _LIVENESS_H
#define _LIVENESS_H

#include "Optimizer/ssa/cfg_info.h"

typedef struct _ssa_liveness {
	zend_ssa *ssa;
	const cfg_info *info;
	uint32_t block_set_len;
	zend_bitset reduced_reachable;
	zend_bitset targets;
} ssa_liveness;

void ssa_liveness_precompute(
	zend_optimizer_ctx *opt_ctx, ssa_liveness *liveness, zend_ssa *ssa, cfg_info *info);
zend_bool ssa_is_live_in_at_block(const ssa_liveness *liveness, int var_num, int block);
zend_bool ssa_is_live_in_at_op(const ssa_liveness *liveness, int var_num, int op);
zend_bool ssa_is_live_out_at_op(const ssa_liveness *liveness, int var_num, int op);

#endif

