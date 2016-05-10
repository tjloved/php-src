#ifndef _SSA_SCP_H
#define _SSA_SCP_H

/* The SCP structures and functions are exported to allow running SCP in parallel to other
 * analysis passes based on the SCDF framework. */

typedef struct _scp_ctx {
	zend_op_array *op_array;
	zend_ssa *ssa;
	zend_call_info **call_map;
	zval *values;
	zval top;
	zval bot;
} scp_ctx;

void scp_context_init(scp_ctx *ctx,
		zend_ssa *ssa, zend_op_array *op_array, zend_call_info **call_map);
void scp_context_free(scp_ctx *ctx);

void scp_visit_instr(scdf_ctx *scdf, void *void_ctx, zend_op *opline, zend_ssa_op *ssa_op);
void scp_visit_phi(scdf_ctx *scdf, void *void_ctx, zend_ssa_phi *phi);
zend_bool scp_get_feasible_successors(
		scdf_ctx *scdf, void *void_ctx, zend_basic_block *block,
		zend_op *opline, zend_ssa_op *ssa_op, zend_bool *suc);

#endif
