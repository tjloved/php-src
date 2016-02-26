#ifndef _SSA_PASS_H
#define _SSA_PASS_H

#include "Optimizer/ssa/helpers.h"

typedef struct _ssa_opt_ctx {
	zend_optimizer_ctx *opt_ctx;
	zend_op_array *op_array;
	zend_ssa *ssa;
	struct _cfg_info *cfg_info;
	struct _ssa_liveness *liveness;
	zend_func_info *func_info;
	zend_call_info **call_map;
	unsigned reorder_dtor_effects : 1;
} ssa_opt_ctx;

void ssa_optimize_scp(ssa_opt_ctx *ctx);
void ssa_optimize_dce(ssa_opt_ctx *ctx);
void ssa_optimize_copy(ssa_opt_ctx *ctx);
void ssa_optimize_assign(ssa_opt_ctx *ctx);
void ssa_optimize_gvn(ssa_opt_ctx *ctx);
void ssa_optimize_cv_to_tmp(ssa_opt_ctx *ctx);
void ssa_optimize_type_specialization(ssa_opt_ctx *ctx);
void ssa_optimize_object_specialization(ssa_opt_ctx *ctx);
void ssa_optimize_compact_vars(ssa_opt_ctx *ctx);
void ssa_optimize_destroy_ssa(ssa_opt_ctx *ctx);

int ssa_verify_integrity(zend_ssa *ssa, const char *extra);

#endif

