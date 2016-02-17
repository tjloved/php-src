#ifndef _CFG_INFO_H
#define _CFG_INFO_H

typedef struct _cfg_info {
	uint32_t *preorder;
	uint32_t *postorder;
	zend_bitset backedges;
	zend_bitset backedge_targets;
	zend_bitset backedge_sources;
	zend_bitset sdom;
	uint32_t block_set_len;
} cfg_info;

void compute_cfg_info(cfg_info *info, zend_optimizer_ctx *opt_ctx, const zend_cfg *cfg);

static inline zend_bool block_strictly_dominates(const cfg_info *info, int a, int b) {
	return zend_bitset_in(info->sdom + a * info->block_set_len, b);
}
static inline zend_bool block_dominates(const cfg_info *info, int a, int b) {
	return a == b || block_strictly_dominates(info, a, b);
}

static inline zend_bool block_is_backedge_target(const cfg_info *info, int block) {
	return zend_bitset_in(info->backedge_targets, block);
}

#endif

