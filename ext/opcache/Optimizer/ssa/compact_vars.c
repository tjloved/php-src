#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/statistics.h"

void ssa_optimize_compact_vars(ssa_opt_ctx *ctx) {
	zend_op_array *op_array = ctx->op_array;
	int i;

	ALLOCA_FLAG(use_heap1);
	ALLOCA_FLAG(use_heap2);
	uint32_t used_cvs_len = zend_bitset_len(op_array->last_var);
	zend_bitset used_cvs = ZEND_BITSET_ALLOCA(used_cvs_len, use_heap1);
	uint32_t *cv_map = do_alloca(op_array->last_var * sizeof(uint32_t), use_heap2);
	uint32_t num_cvs, tmp_offset;

	/* Determine which CVs are used */
	zend_bitset_clear(used_cvs, used_cvs_len);
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		if (opline->op1_type == IS_CV) {
			zend_bitset_incl(used_cvs, VAR_NUM(opline->op1.var));
		}
		if (opline->op2_type == IS_CV) {
			zend_bitset_incl(used_cvs, VAR_NUM(opline->op2.var));
		}
		if (opline->result_type == IS_CV) {
			zend_bitset_incl(used_cvs, VAR_NUM(opline->result.var));
		}
	}

	num_cvs = 0;
	for (i = 0; i < op_array->last_var; i++) {
		if (zend_bitset_in(used_cvs, i)) {
			cv_map[i] = num_cvs++;
		} else {
			cv_map[i] = (uint32_t) -1;
		}
	}

	free_alloca(used_cvs, use_heap1);
	if (num_cvs == op_array->last_var) {
		free_alloca(cv_map, use_heap2);
		OPT_STAT(vars_orig_cvs) += op_array->last_var;
		return;
	}

	ZEND_ASSERT(num_cvs < op_array->last_var);
	tmp_offset = op_array->last_var - num_cvs;

	/* Update CV and TMP references in opcodes */
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		if (opline->op1_type == IS_CV) {
			opline->op1.var = NUM_VAR(cv_map[VAR_NUM(opline->op1.var)]);
		} else if (opline->op1_type & (IS_VAR|IS_TMP_VAR)) {
			opline->op1.var -= sizeof(zval) * tmp_offset;
		}
		if (opline->op2_type == IS_CV) {
			opline->op2.var = NUM_VAR(cv_map[VAR_NUM(opline->op2.var)]);
		} else if (opline->op2_type & (IS_VAR|IS_TMP_VAR)) {
			opline->op2.var -= sizeof(zval) * tmp_offset;
		}
		if (opline->result_type == IS_CV) {
			opline->result.var = NUM_VAR(cv_map[VAR_NUM(opline->result.var)]);
		} else if (opline->result_type & (IS_VAR|IS_TMP_VAR)) {
			opline->result.var -= sizeof(zval) * tmp_offset;
		}
	}

	/* Update TMP references in live ranges */
	if (op_array->live_range) {
		for (i = 0; i < op_array->last_live_range; i++) {
			op_array->live_range[i].var -= sizeof(zval) * tmp_offset;
		}
	}

	/* Update CV name table */
	{
		zend_string **names = safe_emalloc(sizeof(zend_string *), num_cvs, 0);
		for (i = 0; i < op_array->last_var; i++) {
			if (cv_map[i] != (uint32_t) -1) {
				names[cv_map[i]] = op_array->vars[i];
			} else {
				zend_string_release(op_array->vars[i]);
			}
		}
		efree(op_array->vars);
		op_array->vars = names;
	}

	/* Update $this reference */
	if (op_array->this_var != (uint32_t) -1) {
		op_array->this_var = NUM_VAR(cv_map[VAR_NUM(op_array->this_var)]);
	}

	OPT_STAT(vars_orig_cvs) += op_array->last_var;
	OPT_STAT(vars_dead_cvs) += op_array->last_var - num_cvs;
	op_array->last_var = num_cvs;

	free_alloca(cv_map, use_heap2);
}
