#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/cfg_info.h"
#include "Optimizer/statistics.h"

/* GVN instruction encoding:
 *
 * 64-bit [op:8] [op1:28] [op2:28]
 * 32-bit [op:8] [op1:12] [op2:12]
 *
 * The ops are value numbers, where a value number can be either a ssa var number, the TOP
 * element (congruent with all valnums) or a value number corresponding to a constant. The
 * mapping constant to valnum for the latter case is maintained by another hashtable.
 *
 * We use the largest possible valnum as TOP.
 */

#if SIZEOF_ZEND_LONG == 8
# define TOP 0xFFFFFFF
# define OP_WIDTH 28
#else
# define TOP 0xFFF
# define OP_WIDTH 12
#endif

/* Used to signal operands that cannot be numbered. This includes operands that are references,
 * and unsupported literal types. */
#define INVALID 0xFFFFFFFF

typedef struct _context {
	zend_arena *arena;
	zend_op_array *op_array;
	zend_ssa *ssa;
	const cfg_info *info;
	HashTable hash;

	uint32_t *valnums;
	uint32_t max_num;
	uint32_t null_num;
	uint32_t false_num;
	uint32_t true_num;
	HashTable const_valnums;
} context;

static inline zend_bool can_gvn(const zend_ssa_op *ssa_op, const zend_op *opline) {
	if (ssa_op->result_use >= 0) {
		return 0;
	}
	if (ssa_op->op2_def >= 0) {
		return 0;
	}

	switch (opline->opcode) {
		case ZEND_ASSIGN:
		case ZEND_QM_ASSIGN:
			return 1;
		default:
			return 0;
	}
}

static inline uint32_t get_const_valnum(context *ctx, zval *val) {
	switch (Z_TYPE_P(val)) {
		case IS_NULL:
			return ctx->null_num;
		case IS_FALSE:
			return ctx->false_num;
		case IS_TRUE:
			return ctx->true_num;
		case IS_LONG:
		{
			zval *zv = zend_hash_index_find(&ctx->const_valnums, Z_LVAL_P(val));
			if (zv) {
				return Z_LVAL_P(zv);
			} else if (ctx->max_num < TOP) {
				zval new_zv;
				ZVAL_LONG(&new_zv, ctx->max_num);
				zend_hash_index_add_new(&ctx->const_valnums, Z_LVAL_P(val), &new_zv);
				return ctx->max_num++;
			} else {
				return INVALID;
			}
		}
		case IS_STRING:
		{
			zval *zv = zend_hash_find(&ctx->const_valnums, Z_STR_P(val));
			if (zv) {
				return Z_LVAL_P(zv);
			} else if (ctx->max_num < TOP) {
				zval new_zv;
				ZVAL_LONG(&new_zv, ctx->max_num);
				zend_hash_add_new(&ctx->const_valnums, Z_STR_P(val), &new_zv);
				return ctx->max_num++;
			} else {
				return INVALID;
			}
		}
		// TODO: IS_DOUBLE
		default:
			return INVALID;
	}
}

static inline uint32_t get_valnum(const context *ctx, int var_num) {
	ZEND_ASSERT(ctx->valnums[var_num] != TOP);
	return ctx->valnums[var_num];
}

static inline uint32_t get_op1_valnum(
		context *ctx, const zend_ssa_op *ssa_op, const zend_op *opline) {
	if (ssa_op->op1_use >= 0) {
		return get_valnum(ctx, ssa_op->op1_use);
	} else if (opline->op1_type == IS_CONST) {
		return get_const_valnum(ctx, CT_CONSTANT_EX(ctx->op_array, opline->op1.constant));
	} else {
		// TODO Reconsider this when GVNing ops that are maybe UNUSED
		return TOP;
	}
}

static inline uint32_t get_op2_valnum(
		context *ctx, const zend_ssa_op *ssa_op, const zend_op *opline) {
	if (ssa_op->op2_use >= 0) {
		return get_valnum(ctx, ssa_op->op2_use);
	} else if (opline->op2_type == IS_CONST) {
		return get_const_valnum(ctx, CT_CONSTANT_EX(ctx->op_array, opline->op2.constant));
	} else {
		// TODO Reconsider this when GVNing ops that are maybe UNUSED
		return TOP;
	}
}

static inline zend_ulong hash_instr(
		context *ctx, const zend_ssa_op *ssa_op, const zend_op *opline) {
	uint32_t op1_num = get_op1_valnum(ctx, ssa_op, opline);
	uint32_t op2_num = get_op2_valnum(ctx, ssa_op, opline);
	if (op1_num == INVALID || op2_num == INVALID) {
		return -Z_UL(1);
	}
	return (((zend_ulong) opline->opcode) << (2 * OP_WIDTH)) | (op1_num << OP_WIDTH) | op2_num;
}

static inline zend_bool handle_instr(context *ctx, zend_ssa_op *ssa_op, zend_op *opline) {
	zend_bool changed = 0;

	if (ssa_op->op1_def >= 0 && ctx->valnums[ssa_op->op1_def] != ssa_op->op1_def) {
		uint32_t valnum;
		if (opline->opcode == ZEND_ASSIGN) {
			uint32_t rhs_num = get_op2_valnum(ctx, ssa_op, opline);
			valnum = rhs_num != INVALID ? rhs_num : ssa_op->op1_def;
		} else {
			valnum = ssa_op->op1_def;
		}
		if (ctx->valnums[ssa_op->op1_def] != valnum) {
			ctx->valnums[ssa_op->op1_def] = valnum;
			changed = 1;
		}
	}

	if (ssa_op->result_def >= 0 && ctx->valnums[ssa_op->result_def] != ssa_op->result_def) {
		uint32_t valnum;
		if (opline->opcode == ZEND_QM_ASSIGN) {
			uint32_t rhs_num = get_op1_valnum(ctx, ssa_op, opline);
			valnum = rhs_num != INVALID ? rhs_num : ssa_op->result_def;
		} else if (opline->opcode == ZEND_ASSIGN) {
			uint32_t rhs_num = get_op2_valnum(ctx, ssa_op, opline);
			valnum = rhs_num != INVALID ? rhs_num : ssa_op->result_def;
		} else {
			valnum = ssa_op->result_def;
		}
		if (ctx->valnums[ssa_op->result_def] != valnum) {
			ctx->valnums[ssa_op->result_def] = valnum;
			changed = 1;
		}
	}

	return changed;
}

static inline zend_bool handle_phi(context *ctx) {
	return 0;
}

static void init_valnums(context *ctx) {
	const zend_op_array *op_array = ctx->op_array;
	const zend_ssa *ssa = ctx->ssa;
	int i;

	ctx->max_num = ssa->vars_count;
	ctx->null_num = ctx->max_num++;
	ctx->false_num = ctx->max_num++;
	ctx->true_num = ctx->max_num++;
	zend_hash_init(&ctx->const_valnums, 0, NULL, NULL, 0);

	for (i = 0; i < ssa->vars_count; i++) {
		const zend_ssa_var *var = &ssa->vars[i];

		/* Instructions that cannot be GVNd trivially map to themselves */
		int def = var->definition;
		if (def >= 0 && !can_gvn(&ssa->ops[def], &op_array->opcodes[def])) {
			ctx->valnums[i] = i;
			continue;
		}

		/* Reference variables cannot participate in GVN at all */
		if (ssa->var_info[i].type & MAY_BE_REF) {
			ctx->valnums[i] = INVALID;
			continue;
		}

		/* Everything else is optimistically initialized to TOP */
		ctx->valnums[i] = TOP;
	}
}

static void gvn_solve(context *ctx) {
	const zend_op_array *op_array = ctx->op_array;
	const zend_ssa *ssa = ctx->ssa;
	const zend_cfg *cfg = &ssa->cfg;
	const uint32_t *postorder = ctx->info->postorder;

	zend_bool changed = 0;
	do {
		int i;
		/* Traverse CFG in RPO */
		for (i = cfg->blocks_count - 1; i >= 0; i--) {
			uint32_t n = postorder[i];
			const zend_basic_block *block = &cfg->blocks[n];
			const zend_ssa_block *ssa_block = &ssa->blocks[n];
			int j;
			zend_ssa_phi *phi;
			for (j = block->start; j < block->end; j++) {
				changed |= handle_instr(ctx, &ssa->ops[j], &op_array->opcodes[j]);
			}
			for (phi = ssa_block->phis; phi; phi = phi->next) {
				changed |= handle_phi(ctx);
			}
		}
		zend_hash_clean(&ctx->hash);
	} while (changed);
}

void ssa_optimize_gvn(ssa_opt_ctx *ssa_ctx) {
	void *checkpoint = zend_arena_checkpoint(ssa_ctx->opt_ctx->arena);
	context ctx;

	if (ctx.ssa->vars_count >= TOP) {
		/* Our instruction encoding only has limited valnum space */
		return;
	}

	ctx.arena = ssa_ctx->opt_ctx->arena;
	ctx.op_array = ssa_ctx->op_array;
	ctx.ssa = ssa_ctx->ssa;
	ctx.info = ssa_ctx->cfg_info;
	ctx.valnums = zend_arena_alloc(&ctx.arena, sizeof(int32_t) * ctx.ssa->vars_count);
	zend_hash_init(&ctx.hash, 0, NULL, NULL, 0); // TODO size estimate

	init_valnums(&ctx);
	gvn_solve(&ctx);

	zend_hash_destroy(&ctx.hash);
	zend_arena_release(&ctx.arena, checkpoint);
	ssa_ctx->opt_ctx->arena = ctx.arena;
}

