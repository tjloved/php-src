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
 *  * If ENSURE_HAVE_THIS dominates another ENSURE_HAVE_THIS, the latter may be dropped. We only
 *    need to ensure this once.
 */

typedef struct _arginfo {
	zend_string *lcname;
	uint32_t arg_offset;
} arginfo;

typedef struct _context {
	zend_cfg *cfg;
	arginfo *info;
	uint32_t arginfo_pos;
	zend_bool have_this_check;
} context;

static inline zend_bool have_arginfo(
		const context *ctx, zend_string *lcname, uint32_t arg_offset) {
	uint32_t i;
	for (i = 0; i < ctx->arginfo_pos; i++) {
		if (zend_string_equals(ctx->info[i].lcname, lcname)
				&& ctx->info[i].arg_offset == arg_offset) {
			return 1;
		}
	}
	return 0;
}

static void process_block(context *ctx, zend_op_array *op_array, zend_basic_block *block) {
	zend_op *opline = &op_array->opcodes[block->start];
	zend_op *end = opline + block->len;
	for (; opline < end; opline++) {
		if (opline->opcode == ZEND_INIT_FCALL_BY_NAME) {
			// TODO: We're effectively skipping the opcodes of the argument list here
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
							ctx->info[ctx->arginfo_pos].lcname = lcname;
							ctx->info[ctx->arginfo_pos].arg_offset = opline->op2.num;
							ctx->arginfo_pos++;
						}
					} else if (opline->opcode == ZEND_SEND_VAR_EX) {
						if (have_arginfo(ctx, lcname, opline->op2.num)) {
							opline->opcode = ZEND_SEND_VAR;
							OPT_STAT(simplified_sends)++;
						}
					}
				}
			}
		} else if (opline->opcode == ZEND_ENSURE_HAVE_THIS) {
			if (ctx->have_this_check) {
				MAKE_NOP(opline);
			}
			ctx->have_this_check = 1;
		}
	}
}

static void propagate_recursive(context *ctx, zend_op_array *op_array, int block_num) {
	zend_cfg *cfg = ctx->cfg;
	zend_basic_block *block = &cfg->blocks[block_num];
	uint32_t orig_arginfo_pos = ctx->arginfo_pos;
	zend_bool orig_have_this_check = ctx->have_this_check;
	int i;

	process_block(ctx, op_array, block);

	for (i = cfg->blocks[block_num].children; i >= 0; i = cfg->blocks[i].next_child) {
		propagate_recursive(ctx, op_array, i);
	}

	/* Reset state when going back up the domtree */
	ctx->arginfo_pos = orig_arginfo_pos;
	ctx->have_this_check = orig_have_this_check;
}

void ssa_propagate_along_domtree(zend_op_array *op_array, zend_cfg *cfg) {
	arginfo *info = safe_emalloc(sizeof(arginfo), op_array->last, 0);
	context ctx = {cfg, info, 0, 0};
	propagate_recursive(&ctx, op_array, 0);
	efree(info);
}
