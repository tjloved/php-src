#ifndef _CFG_INFO_H
#define _CFG_INFO_H

typedef struct _cfg_info {
	uint32_t *preorder;
	uint32_t *postorder;
	zend_bitset backedges;
	zend_bitset backedge_targets;
	zend_bitset backedge_sources;
} cfg_info;

void compute_cfg_info(cfg_info *info, zend_optimizer_ctx *opt_ctx, const zend_cfg *cfg);

#endif

