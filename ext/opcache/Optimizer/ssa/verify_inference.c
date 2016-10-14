#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/instructions.h"
#include "Optimizer/statistics.h"

typedef struct _context {
	zend_op_array *op_array;
	zend_ssa *ssa;
	int *block_shiftlist;
	int *shiftlist;
	uint32_t new_num_opcodes;
} context;

static inline uint32_t num_def_operands(zend_ssa_op *ssa_op) {
	return (ssa_op->result_def >= 0) + (ssa_op->op1_def >= 0) + (ssa_op->op2_def >= 0);
}

static void emit_type_check(
		zend_ssa *ssa, zend_op *opline, int var, int op_type, int ssa_var, uint32_t lineno) {
	uint32_t type = ssa->var_info[ssa_var].type;

	if (type & MAY_BE_CLASS) {
		/* No way to check this one at runtime */
		MAKE_NOP(opline);
		return;
	}

	opline->opcode = ZEND_ASSERT_TYPE;
	opline->op1_type = op_type;
	opline->op1.var = var;
	opline->op2_type = IS_UNUSED;
	opline->result_type = IS_UNUSED;
	opline->extended_value = type;
	opline->lineno = lineno;
}

static zend_op *emit_type_checks(
		zend_ssa *ssa, zend_op *new_opline, zend_op *opline, zend_ssa_op *ssa_op) {
	if (ssa_op->result_def >= 0) {
		emit_type_check(ssa, new_opline,
			opline->result.var, opline->result_type, ssa_op->result_def, opline->lineno);
		new_opline++;
	}
	if (ssa_op->op1_def >= 0) {
		emit_type_check(ssa, new_opline,
			opline->op1.var, opline->op1_type, ssa_op->op1_def, opline->lineno);
		new_opline++;
	}
	if (ssa_op->op2_def >= 0) {
		emit_type_check(ssa, new_opline,
			opline->op2.var, opline->op2_type, ssa_op->op2_def, opline->lineno);
		new_opline++;
	}
	if (ssa_op->op1_def >= 0 && ssa_op->op2_def >= 0
			&& ssa->vars[ssa_op->op1_def].var == ssa->vars[ssa_op->op2_def].var) {
		/* Handle super special case where the same var is def'd twice */
		if (ssa_op->op1_def > ssa_op->op2_def) {
			MAKE_NOP(new_opline - 1);
		} else {
			MAKE_NOP(new_opline - 2);
		}
	}
	return new_opline;
}

static zend_bool should_skip(zend_op *opline, zend_op *end) {
	if (opline+1 != end && (opline+1)->opcode == ZEND_OP_DATA) {
		/* Can't insert instructions before OP_DATA */
		return 1;
	}
	switch (opline->opcode) {
		/* These don't return real strings */
		case ZEND_ROPE_INIT:
		case ZEND_ROPE_ADD:
			return 1;
		/* Branches with a result */
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_ASSERT_CHECK:
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
		/* The final result is only available after multiple instructions */
		case ZEND_FETCH_DIM_W:
		case ZEND_FETCH_DIM_RW:
		case ZEND_FETCH_DIM_FUNC_ARG:
			return 1;
	}
	return 0;
}

static void compute_shiftlist(context *ctx) {
	zend_ssa *ssa = ctx->ssa;
	zend_cfg *cfg = &ssa->cfg;
	int *block_shiftlist = ctx->block_shiftlist;
	int *shiftlist = ctx->shiftlist;
	zend_op *end = ctx->op_array->opcodes + ctx->op_array->last;
	int i, j, shift = 0;

	ctx->new_num_opcodes = 0;
	for (i = 0; i < cfg->blocks_count; i++) {
		const zend_basic_block *block = &cfg->blocks[i];
		block_shiftlist[i] = shift;

		if (!(block->flags & ZEND_BB_REACHABLE) || block->len == 0) {
			shift -= block->len;
			continue;
		}

		ctx->new_num_opcodes += block->len;
		for (j = block->start; j < block->start + block->len; j++) {
			zend_ssa_op *ssa_op = &ssa->ops[j];
			uint32_t num = num_def_operands(ssa_op);
			shiftlist[j] = shift;
			if (!should_skip(&ctx->op_array->opcodes[j], end)) {
				shift += num;
				ctx->new_num_opcodes += num;
			}
		}
	}
}

static void copy_instr(
		context *ctx, zend_op *new_opline, zend_op *old_opline, zend_op *new_opcodes) {
#define TO_NEW(block_num) \
		(new_opcodes + cfg->blocks[block_num].start + ctx->block_shiftlist[block_num])
	zend_op_array *op_array = ctx->op_array;
	const zend_cfg *cfg = &ctx->ssa->cfg;
	zend_basic_block *block = &cfg->blocks[cfg->map[old_opline - op_array->opcodes]];
	*new_opline = *old_opline;

	/* Adjust JMP offsets */
	switch (new_opline->opcode) {
		case ZEND_JMP:
		case ZEND_FAST_CALL:
			ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op1, TO_NEW(block->successors[0]));
			break;
		case ZEND_JMPZNZ:
			new_opline->extended_value = ZEND_OPLINE_TO_OFFSET(
				new_opline, TO_NEW(block->successors[1]));
			/* break missing intentionally */
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_ASSERT_CHECK:
			ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op2, TO_NEW(block->successors[0]));
			break;
		case ZEND_CATCH:
			if (!new_opline->result.num) {
				new_opline->extended_value = ZEND_OPLINE_TO_OFFSET(
					new_opline, TO_NEW(block->successors[0]));
			}
			break;
		case ZEND_DECLARE_ANON_CLASS:
		case ZEND_DECLARE_ANON_INHERITED_CLASS:
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
			new_opline->extended_value = ZEND_OPLINE_TO_OFFSET(
				new_opline, TO_NEW(block->successors[0]));
			break;
	} 
#undef TO_NEW
}

static void insert_type_checks(context *ctx) {
	zend_op_array *op_array = ctx->op_array;
	zend_ssa *ssa = ctx->ssa;
	zend_cfg *cfg = &ssa->cfg;
	const int *shiftlist = ctx->shiftlist;
	zend_op *end = op_array->opcodes + op_array->last;
	zend_op *new_opcodes = ecalloc(sizeof(zend_op), ctx->new_num_opcodes);
	zend_op *new_opline = new_opcodes;
	int i, j;

	// TODO phis
	for (i = 0; i < cfg->blocks_count; i++) {
		const zend_basic_block *block = &cfg->blocks[i];

		if (!(block->flags & ZEND_BB_REACHABLE) || block->len == 0) {
			continue;
		}

		for (j = block->start; j < block->start + block->len; j++) {
			zend_op *opline = &op_array->opcodes[j];
			zend_ssa_op *ssa_op = &ssa->ops[j];
			copy_instr(ctx, new_opline++, opline, new_opcodes);

			/* Need to honor use_as_double if we're adding type checks */
			if (opline->opcode == ZEND_ASSIGN && opline->op2_type == IS_CONST
					&& ssa_op->op1_def >= 0 && ssa->var_info[ssa_op->op1_def].use_as_double) {
				zval *op2 = CT_CONSTANT_EX(op_array, opline->op2.constant);
				convert_to_double(op2);
			}
			if (!should_skip(&ctx->op_array->opcodes[j], end)) {
				new_opline = emit_type_checks(ssa, new_opline, opline, ssa_op);
			}
		}
	}

	efree(op_array->opcodes);
	op_array->opcodes = new_opcodes;
	op_array->last = ctx->new_num_opcodes;

	/* Update live ranges */
	if (op_array->last_live_range) {
		for (i = 0; i < op_array->last_live_range; i++) {
			zend_live_range *range = &op_array->live_range[i];
			range->start += shiftlist[range->start];
			range->end += shiftlist[range->end];
		}
	}

	/* Update early binding chain */
	if (op_array->early_binding != (uint32_t) -1) {
		uint32_t *opline_num = &op_array->early_binding;
		do {
			*opline_num += shiftlist[*opline_num];
			opline_num = &op_array->opcodes[*opline_num].result.opline_num;
		} while (*opline_num != (uint32_t) -1);
	}
}

void ssa_verify_inference(zend_optimizer_ctx *opt_ctx, zend_op_array *op_array, zend_ssa *ssa) {
	context ctx;
	ctx.op_array = op_array;
	ctx.ssa = ssa;
	ctx.block_shiftlist = zend_arena_calloc(&opt_ctx->arena, ssa->cfg.blocks_count, sizeof(int));
	ctx.shiftlist = zend_arena_calloc(&opt_ctx->arena, op_array->last, sizeof(int));
	compute_shiftlist(&ctx);
	insert_type_checks(&ctx);
}
