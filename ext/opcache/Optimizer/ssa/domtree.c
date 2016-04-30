#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/instructions.h"
#include "Optimizer/statistics.h"

/* This file implements optimizations that only need to propagate information along the dominator
 * tree. This pass runs before SSA construction. Currently implemented:
 *  * If SEND_VAL_EX is used for an argument of a ZEND_INIT_FCALL_BY_NAME function, we can
 *    assume that this is a by-value argument in dominated paths and replace
 *    SEND_VAL_EX/SEND_VAR_EX opcodes with SEND_VAL/SEND_VAR opcodes.
 */

typedef struct _arginfo {
	zend_string *lcname;
	uint32_t arg_offset;
} arginfo;

typedef struct _arginfo_context {
	zend_cfg *cfg;
	arginfo *info;
	uint32_t pos;
} arginfo_context;

static inline zend_bool have_arginfo(
		arginfo_context *ctx, zend_string *lcname, uint32_t arg_offset) {
	uint32_t i;
	for (i = 0; i < ctx->pos; i++) {
		if (zend_string_equals(ctx->info[i].lcname, lcname)
				&& ctx->info[i].arg_offset == arg_offset) {
			return 1;
		}
	}
	return 0;
}

static void propagate_recursive(arginfo_context *ctx, zend_op_array *op_array, int block_num) {
	zend_cfg *cfg = ctx->cfg;
	zend_basic_block *block = &cfg->blocks[block_num];
	uint32_t orig_pos = ctx->pos;
	zend_op *opline = &op_array->opcodes[block->start], *end = &op_array->opcodes[block->end];
	int i;
	for (; opline <= end; opline++) {
		if (opline->opcode == ZEND_INIT_FCALL_BY_NAME) {
			zend_string *lcname = Z_STR_P(&ZEND_OP2_LITERAL(opline) + 1);
			int level = 0;
			while (++opline <= end) {
				if (is_init_opline(opline)) {
					level++;
				} else if (is_call_opline(opline)) {
					if (level == 0) {
						break;
					}
					level--;
				}
				if (level == 0) {
					if (opline->opcode == ZEND_SEND_VAL_EX) {
						if (have_arginfo(ctx, lcname, opline->op2.num)) {
							opline->opcode = ZEND_SEND_VAL;
							OPT_STAT(simplified_sends)++;
						} else {
							ctx->info[ctx->pos].lcname = lcname;
							ctx->info[ctx->pos].arg_offset = opline->op2.num;
							ctx->pos++;
						}
					} else if (opline->opcode == ZEND_SEND_VAR_EX) {
						if (have_arginfo(ctx, lcname, opline->op2.num)) {
							opline->opcode = ZEND_SEND_VAR;
							OPT_STAT(simplified_sends)++;
						}
					}
				}
			}
		}
	}
	for (i = cfg->blocks[block_num].children; i >= 0; i = cfg->blocks[i].next_child) {
		propagate_recursive(ctx, op_array, i);
	}
	ctx->pos = orig_pos;
}

void ssa_propagate_along_domtree(zend_op_array *op_array, zend_cfg *cfg) {
	arginfo *info = safe_emalloc(sizeof(arginfo), op_array->last, 0);
	arginfo_context ctx = {cfg, info, 0};
	propagate_recursive(&ctx, op_array, 0);

	efree(info);
}
