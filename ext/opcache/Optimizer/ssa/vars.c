#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/statistics.h"
#include "Optimizer/ssa/liveness.h"

typedef struct {
	int min;
	int next;
} group;

#define PCOPY_NUM_RESERVED_ELEMS 4
typedef struct {
	int from;
	int to;
} pcopy_elem;
typedef struct {
	pcopy_elem *elems;
	pcopy_elem elems_storage[PCOPY_NUM_RESERVED_ELEMS];
	uint32_t num_elems;
} pcopy;
typedef struct {
	pcopy early;
	pcopy late;
} block_info;

typedef struct {
	zend_op_array *op_array;
	zend_ssa *ssa;
	const cfg_info *info;
	const ssa_liveness *liveness;
	int *predom;
	group *groups;
	zend_stack stack;
	block_info *blocks;
	int *shiftlist;
} context;

static zend_bool interfere_dominating(const ssa_liveness *liveness, int a, zend_ssa_var *var_b) {
	if (var_b->definition >= 0) {
		return ssa_is_live_out_at_op(liveness, a, var_b->definition);
	} else if (var_b->definition_phi) {
		return ssa_is_live_in_at_block(liveness, a, var_b->definition_phi->block);
	} else {
		return 1;
	}
}

static zend_bool interfere(const zend_ssa *ssa, const ssa_liveness *liveness, int a, int b) {
	zend_ssa_var *var_a = &ssa->vars[a];
	zend_ssa_var *var_b = &ssa->vars[b];
	if (var_dominates(ssa, liveness->info, var_a, var_b)) {
		//fprintf(stderr, "%d dominates %d\n", a, b);
		return interfere_dominating(liveness, a, var_b);
	} else {
		//fprintf(stderr, "%d dominates %d\n", b, a);
		return interfere_dominating(liveness, b, var_a);
	}
}

/* Compute preorder numbering of SSA vars over dominance relationship */
static int compute_dominance_preorder_recursive(context *ctx, int num, int block_num) {
	zend_ssa *ssa = ctx->ssa;
	zend_basic_block *block = &ssa->cfg.blocks[block_num];
	zend_ssa_block *ssa_block = &ssa->blocks[block_num];
	zend_ssa_phi *phi;
	int i;
	for (phi = ssa_block->phis; phi; phi = phi->next) {
		ctx->predom[phi->ssa_var] = num++;
	}
	for (i = block->start; i <= block->end; i++) {
		zend_ssa_op *ssa_op = &ssa->ops[i];
		if (ssa_op->result_def >= 0) {
			ctx->predom[ssa_op->result_def] = num++;
		}
		if (ssa_op->op1_def >= 0) {
			ctx->predom[ssa_op->op1_def] = num++;
		}
		if (ssa_op->op2_def >= 0) {
			ctx->predom[ssa_op->op2_def] = num++;
		}
	}

	for (i = block->children; i >= 0; i = ssa->cfg.blocks[i].next_child) {
		num = compute_dominance_preorder_recursive(ctx, num, i);
	}
	return num;
}
static void compute_dominance_preorder(context *ctx, ssa_opt_ctx *ssa_ctx) {
	int i;
	for (i = 0; i < ssa_ctx->op_array->last_var; i++) {
		ctx->predom[i] = i;
	}
	compute_dominance_preorder_recursive(ctx, i, 0);
}

static void merge_groups(context *ctx, int var1, int var2) {
	group *groups = ctx->groups;
	int i;

	ZEND_ASSERT(var1 >= 0 && var2 >= 0);
	var1 = groups[var1].min;
	var2 = groups[var2].min;

	/* At least one variable does not participate */
	if (var1 < 0 || var2 < 0) {
		return;
	}

	/* Already in the same group */
	if (var1 == var2) {
		return;
	}

	/* Make sure var1 < var2 */
	if (ctx->predom[var1] > ctx->predom[var2]) {
		int tmp = var1;
		var1 = var2;
		var2 = tmp;
	}

	/* Set min of var2 group to var1 */
	for (i = var2; i != -1; i = groups[i].next) {
		groups[i].min = var1;
	}

	/* Merge lists while keeping predom order */
	i = groups[var1].next;
	while (i != -1 && var2 != -1) {
		if (ctx->predom[i] > ctx->predom[var2]) {
			groups[var1].next = var2;
			var1 = var2;
			var2 = i;
		} else {
			var1 = i;
		}
		i = groups[var1].next;
	}
	if (i == -1) {
		groups[var1].next = var2;
	}
}

static zend_bool group_has_interference(context *ctx, int var) {
	const zend_ssa *ssa = ctx->ssa;
	const group *groups = ctx->groups;
	ZEND_ASSERT(groups[var].min == var);

	do {
		zend_ssa_var *ssa_var = &ssa->vars[var];
		int *top = zend_stack_top(&ctx->stack);
		int other = top ? *top : -1;
		while (other != -1 && var_dominates(ctx->ssa, ctx->info, &ssa->vars[other], ssa_var)) {
			zend_stack_del_top(&ctx->stack);
			top = zend_stack_top(&ctx->stack);
			other = top ? *top : -1;
		}

		if (other != -1 && interfere_dominating(ctx->liveness, other, ssa_var)) {
			ctx->stack.top = ctx->stack.max = 0;
			return 1;
		}

		zend_stack_push(&ctx->stack, &var);
		var = groups[var].next;
	} while (var != -1);

	ctx->stack.top = ctx->stack.max = 0;
	return 0;
}

static zend_bool groups_interfere(context *ctx, int var_a, int var_b) {
	const zend_ssa *ssa = ctx->ssa;
	const group *groups = ctx->groups;

	ZEND_ASSERT(var_a >= 0 && var_b >= 0);
	ZEND_ASSERT(groups[var_a].min == var_a);
	ZEND_ASSERT(groups[var_b].min == var_b);

	do {
		int current;
		zend_ssa_var *current_var;
		int *top = zend_stack_top(&ctx->stack);
		int other = top ? *top : -1;

		if (var_a == -1 || (var_b != -1 && ctx->predom[var_b] < ctx->predom[var_a])) {
			current = var_b;
			var_b = groups[var_b].next;
		} else {
			current = var_a;
			var_a = groups[var_a].next;
		}
		current_var = &ssa->vars[current];

		// TODO faster predom based check
		while (other != -1 && var_dominates(ctx->ssa, ctx->info, &ssa->vars[other], current_var)) {
			zend_stack_del_top(&ctx->stack);
			top = zend_stack_top(&ctx->stack);
			other = top ? *top : -1;
		}

		if (other != -1 && groups[other].min != groups[current].min
				&& interfere_dominating(ctx->liveness, other, current_var)) {
			ctx->stack.top = ctx->stack.max = 0;
			return 1;
		}

		zend_stack_push(&ctx->stack, &current);
	} while (var_a != -1 || var_b != -1);

	ctx->stack.top = ctx->stack.max = 0;
	return 0;
}

static inline void try_merge(context *ctx, int a, int b) {
	a = ctx->groups[a].min;
	b = ctx->groups[b].min;
	if (a == -1 || b == -1 || a == b) {
		return;
	}
	if (groups_interfere(ctx, a, b)) {
		fprintf(stderr, "Groups %d and %d interfere\n", a, b);
		return;
	}

	merge_groups(ctx, a, b);
}

static void remove_essa_pis(zend_ssa *ssa) {
	zend_ssa_phi *phi;
	FOREACH_PHI(phi) {
		if (phi->pi >= 0) {
			rename_var_uses(ssa, phi->ssa_var, phi->sources[0]);
			remove_phi(ssa, phi);
		}
	} FOREACH_PHI_END();
}

static inline void pcopy_init(pcopy *cpy) {
	cpy->num_elems = 0;
	cpy->elems = cpy->elems_storage;
}

static inline zend_bool is_pow2(uint32_t n) {
	return n != 0 && !(n & (n - 1));
}

static inline pcopy_elem *pcopy_reserve_elem(pcopy *cpy) {
	if (cpy->num_elems >= PCOPY_NUM_RESERVED_ELEMS && is_pow2(cpy->num_elems)) {
		// TODO How common?
		pcopy_elem *new_elems = emalloc(2 * sizeof(pcopy_elem) * cpy->num_elems);
		memcpy(new_elems, cpy->elems, sizeof(pcopy_elem) * cpy->num_elems);
		if (cpy->elems != cpy->elems_storage) {
			efree(cpy->elems);
		}
		cpy->elems = new_elems;
		//OPT_STAT(tmp)++;
	}
	//OPT_STAT(tmp2)++;

	return &cpy->elems[cpy->num_elems++];
}

static inline void pcopy_add_elem(pcopy *cpy, int from, int to) {
	pcopy_elem *elem = pcopy_reserve_elem(cpy);
	elem->from = from;
	elem->to = to;
}

static inline void pcopy_free(pcopy *cpy) {
	if (cpy->elems != cpy->elems_storage) {
		efree(cpy->elems);
	}
}

static inline uint32_t pcopy_estimated_num_copies(pcopy *cpy) {
	/* A *very* conservative estimate */
	return (cpy->num_elems + 1) / 2 * 3;
}

static void init_block_pcopys(context *ctx) {
	zend_cfg *cfg = &ctx->ssa->cfg;
	int i;
	for (i = 0; i < cfg->blocks_count; i++) {
		pcopy_init(&ctx->blocks[i].early);
		pcopy_init(&ctx->blocks[i].late);
	}
}

static void free_block_pcopys(context *ctx) {
	zend_cfg *cfg = &ctx->ssa->cfg;
	int i;
	for (i = 0; i < cfg->blocks_count; i++) {
		pcopy_free(&ctx->blocks[i].early);
		pcopy_free(&ctx->blocks[i].late);
	}
}

static void collect_pcopys(context *ctx) {
	zend_ssa *ssa = ctx->ssa;
	zend_cfg *cfg = &ssa->cfg;
	zend_ssa_phi *phi;
	FOREACH_PHI(phi) {
		int i, *predecessors;

		if (phi->var >= ssa->op_array->last_var) {
			continue;
		}

		predecessors = &cfg->predecessors[cfg->blocks[phi->block].predecessor_offset];
		if (ssa->var_info[phi->ssa_var].type & MAY_BE_REF) {
			/* References do not participate in copy propagation (etc). We may only have to
			 * insert copies if one of the source operands is a non-ref. */
			for (i = 0; i < cfg->blocks[phi->block].predecessors_count; i++) {
				if (phi->sources[i] >= 0 && !(ssa->var_info[phi->sources[i]].type & MAY_BE_REF)) {
					pcopy_add_elem(
						&ctx->blocks[predecessors[i]].late,
						ssa->vars[phi->sources[i]].var, phi->var);
				}
			}
		} else {
			/* Ordinary, non-reference variables */
			pcopy_add_elem(&ctx->blocks[phi->block].early, 0, phi->var);

			for (i = 0; i < cfg->blocks[phi->block].predecessors_count; i++) {
				if (phi->sources[i] >= 0) {
					pcopy_add_elem(
						&ctx->blocks[predecessors[i]].late,
						ssa->vars[phi->sources[i]].var, 0);
				}
			}
		}
	} FOREACH_PHI_END();
}

static inline zend_bool has_jump_instr(
		const zend_op_array *op_array, const zend_basic_block *block) {
	return block->successors[1] >= 0 || op_array->opcodes[block->end].opcode == ZEND_JMP;
}

static void compute_shiftlists(context *ctx) {
	zend_op_array *op_array = ctx->op_array;
	zend_cfg *cfg = &ctx->ssa->cfg;
	int i, j = 0, shift = 0;
	int *shiftlist = ctx->shiftlist;

	for (i = 0; i < cfg->blocks_count; i++) {
		zend_basic_block *block = &cfg->blocks[i];
		zend_bool has_jump = has_jump_instr(op_array, block);

		while (j < block->start) {
			shiftlist[j++] = shift;
		}

		shift += pcopy_estimated_num_copies(&ctx->blocks[i].early);
		while (j < block->end) {
			shiftlist[j++] = shift;
		}

		if (has_jump) {
			shift += pcopy_estimated_num_copies(&ctx->blocks[i].late);
			shiftlist[j++] = shift;
		} else {
			shiftlist[j++] = shift;
			shift += pcopy_estimated_num_copies(&ctx->blocks[i].late);
		}
	}

	while (j < op_array->last) {
		shiftlist[j++] = shift;
	}
}

static void insert_copies(context *ctx) {
}

static void insert_pcopys(context *ctx) {
	init_block_pcopys(ctx);
	collect_pcopys(ctx);
	compute_shiftlists(ctx);
	insert_copies(ctx);
	free_block_pcopys(ctx);
}

static void ssa_destroy(ssa_opt_ctx *ssa_ctx) {
	zend_ssa *ssa = ssa_ctx->ssa;
	zend_op_array *op_array = ssa_ctx->op_array;
	zend_ssa_phi *phi;
	int i;

	context ctx;
	ctx.op_array = op_array;
	ctx.ssa = ssa;
	ctx.info = ssa_ctx->cfg_info;
	ctx.liveness = ssa_ctx->liveness;
	ctx.predom = zend_arena_alloc(&ssa_ctx->opt_ctx->arena, sizeof(int) * ssa->vars_count);
	ctx.groups = zend_arena_alloc(&ssa_ctx->opt_ctx->arena, sizeof(group) * ssa->vars_count);
	ctx.blocks = zend_arena_alloc(&ssa_ctx->opt_ctx->arena,
			sizeof(block_info) * ssa->cfg.blocks_count);
	ctx.shiftlist = zend_arena_alloc(&ssa_ctx->opt_ctx->arena, sizeof(int) * op_array->last);
	zend_stack_init(&ctx.stack, sizeof(int));

	remove_essa_pis(ssa);

	compute_dominance_preorder(&ctx, ssa_ctx);

	insert_pcopys(&ctx);

	/* Start with identity groups */
	for (i = 0; i < ssa->vars_count; i++) {
		zend_ssa_var *var = &ssa->vars[i];
		ctx.groups[i].next = -1;
		if (var->definition >= 0 || var->definition_phi) {
			ctx.groups[i].min = i;
		} else {
			ctx.groups[i].min = -1;
		}
	}

	FOREACH_PHI(phi) {
		int source;
		FOREACH_PHI_SOURCE(phi, source) {
			try_merge(&ctx, phi->ssa_var, source);
		} FOREACH_PHI_SOURCE_END();
	} FOREACH_PHI_END();

	for (i = 0; i < op_array->last; i++) {
		zend_ssa_op *ssa_op = &ssa->ops[i];
		if (ssa_op->result_use >= 0 && ssa_op->result_def >= 0) {
			try_merge(&ctx, ssa_op->result_use, ssa_op->result_def);
		}
		if (ssa_op->op1_use >= 0 && ssa_op->op1_def >= 0) {
			try_merge(&ctx, ssa_op->op1_use, ssa_op->op1_def);
		}
		if (ssa_op->op2_use >= 0 && ssa_op->op2_def >= 0) {
			try_merge(&ctx, ssa_op->op2_use, ssa_op->op2_def);
		}
	}

	for (i = 0; i < ssa->vars_count; i++) {
		if (ctx.groups[i].min != i) {
			continue;
		}

		if (group_has_interference(&ctx, i)) {
			fprintf(stderr, "Interference in group %d\n", i);
		}
	}

	zend_stack_destroy(&ctx.stack);
}

static void check_interferences(ssa_opt_ctx *ssa_ctx) {
	zend_ssa *ssa = ssa_ctx->ssa;
	ssa_liveness *liveness = ssa_ctx->liveness;
	int last_var = ssa_ctx->op_array->last_var;
	int i, j;

	for (i = 0; i < ssa->vars_count; i++) {
		zend_ssa_var *var_i = &ssa->vars[i];
		int var = var_i->var;
		if (var >= last_var || (var_i->definition < 0 && !var_i->definition_phi)) {
			continue;
		}

		for (j = 0; j < i; j++) {
			zend_ssa_var *var_j = &ssa->vars[j];
			if (var_j->var != var || (var_j->definition < 0 && !var_j->definition_phi)) {
				continue;
			}

			if (interfere(ssa, liveness, i, j)) {
				fprintf(stderr,
					"%d and %d for var %d ($%s) interfere\n",
					i, j, var, ZSTR_VAL(ssa_ctx->op_array->vars[var]));
			}
		}
	}
}

void ssa_optimize_vars(ssa_opt_ctx *ctx) {
	// TODO temporarily in here...
	check_interferences(ctx); 
	ssa_destroy(ctx);

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
