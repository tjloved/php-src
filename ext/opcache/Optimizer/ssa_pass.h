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
	unsigned reorder_dtor_effects : 1;
	unsigned assume_no_undef : 1;
} ssa_opt_ctx;

void ssa_propagate_along_domtree(zend_op_array *op_array, zend_cfg *cfg);
void ssa_optimize_scp(ssa_opt_ctx *ctx);
void ssa_optimize_dce(ssa_opt_ctx *ctx);
void ssa_optimize_simplify_cfg(ssa_opt_ctx *ssa_ctx);
void ssa_optimize_copy(ssa_opt_ctx *ctx);
void ssa_optimize_assign(ssa_opt_ctx *ctx);
void ssa_optimize_gvn(ssa_opt_ctx *ctx);
void ssa_optimize_type_specialization(ssa_opt_ctx *ctx);
void ssa_optimize_object_specialization(ssa_opt_ctx *ctx);
void ssa_optimize_compact_vars(ssa_opt_ctx *ctx);
void ssa_optimize_destroy_ssa(ssa_opt_ctx *ctx);
void ssa_optimize_misc(ssa_opt_ctx *ctx);

int ssa_verify_integrity(zend_ssa *ssa, const char *extra);
void ssa_verify_inference(zend_optimizer_ctx *opt_ctx, zend_op_array *op_array, zend_ssa *ssa);

// 1: After SSA pass
// 2: Before SSA pass
#define SSA_PASS_SCP          (1 <<  2) //    4
#define SSA_PASS_DCE          (1 <<  3) //    8
#define SSA_PASS_SIMPLIFY_CFG (1 <<  4) //   16
#define SSA_PASS_COPY_PROP    (1 <<  5) //   32
#define SSA_PASS_DCE_2        (1 <<  6) //   64
#define SSA_PASS_ASSIGN_CONTR (1 <<  7) //  128
#define SSA_PASS_MISC         (1 <<  8) //  256
#define SSA_PASS_TYPE_SPEC    (1 <<  9) //  512
#define SSA_PASS_OBJ_SPEC     (1 << 10) // 1024

// Combine type inference with SCP
#define SSA_PASS_COMBINE_SCP  (1 << 16)

#endif

