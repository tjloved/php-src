#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/statistics.h"
#include "Optimizer/ssa/liveness.h"

/* This is a work-in-progress implementation of the out-of-ssa translation algorithm described
 * in "Revisiting Out-of-SSA Translation for Correctness, Code Quality, and Efficiency" by
 * Boissinot et al.
 *
 * Far from finished! */

#define DEBUG 0

typedef struct {
	int min;
	int next;
	int cv;
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
	zend_arena *arena;
	zend_op_array *op_array;
	zend_ssa *ssa;
	const cfg_info *info;
	const ssa_liveness *liveness;
	int *predom;
	group *groups;
	zend_stack stack;
	block_info *blocks;
	/* Shiftlist for block starts (used for JMPs) */
	int *block_shiftlist;
	/* Shiftlist for instructions (used for everything else) */
	int *shiftlist;
	int *loc;
	int *pred;
	uint32_t new_num_opcodes;
	uint32_t new_num_vars;
	uint32_t new_num_temps;
	uint32_t num_extra_vars;
	uint32_t num_groups;
	int tmp_var_offset;
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
static void compute_dominance_preorder(context *ctx) {
	int i;
	for (i = 0; i < ctx->op_array->last_var; i++) {
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
	ZEND_ASSERT(a >= 0 && b >= 0);
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

static void coalesce_vars(context *ctx) {
	const zend_ssa *ssa = ctx->ssa;
	const zend_op_array *op_array = ctx->op_array;
	zend_ssa_phi *phi;
	int i;

	for (i = 0; i < ssa->vars_count; i++) {
		zend_ssa_var *var = &ssa->vars[i];
		ctx->groups[i].next = -1;
		ctx->groups[i].cv = -1;

		if (var->var >= op_array->last_var) {
			/* We're only interested in CVs */
			ctx->groups[i].min = -1;
			continue;
		}

		if (var->definition >= 0 || var->definition_phi
				|| (i < op_array->last_var && (var->use_chain >= 0 || var->phi_use_chain))) {
			/* Start with identity groups */
			ctx->groups[i].min = i;
		} else {
			ctx->groups[i].min = -1;
		}
	}

	FOREACH_PHI(phi) {
		int source;
		FOREACH_PHI_SOURCE(phi, source) {
			// TODO This check is here only for experimenting
			if ((ssa->var_info[source].type & MAY_BE_REF)) {
				try_merge(ctx, phi->ssa_var, source);
			}
		} FOREACH_PHI_SOURCE_END();
	} FOREACH_PHI_END();

	for (i = 0; i < op_array->last; i++) {
		zend_ssa_op *ssa_op = &ssa->ops[i];
		if (ssa_op->result_use >= 0 && ssa_op->result_def >= 0) {
			try_merge(ctx, ssa_op->result_use, ssa_op->result_def);
		}
		if (ssa_op->op1_use >= 0 && ssa_op->op1_def >= 0) {
			try_merge(ctx, ssa_op->op1_use, ssa_op->op1_def);
		}
		if (ssa_op->op2_use >= 0 && ssa_op->op2_def >= 0) {
			try_merge(ctx, ssa_op->op2_use, ssa_op->op2_def);
		}
	}

	for (i = 0; i < ssa->vars_count; i++) {
		if (ctx->groups[i].min != i) {
			continue;
		}

		if (group_has_interference(ctx, i)) {
			fprintf(stderr, "Interference in group %d\n", i);
		}
	}
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
	}

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

static void init_block_pcopies(context *ctx) {
	zend_cfg *cfg = &ctx->ssa->cfg;
	int i;
	for (i = 0; i < cfg->blocks_count; i++) {
		pcopy_init(&ctx->blocks[i].early);
		pcopy_init(&ctx->blocks[i].late);
	}
}

static void free_block_pcopies(context *ctx) {
	zend_cfg *cfg = &ctx->ssa->cfg;
	int i;
	for (i = 0; i < cfg->blocks_count; i++) {
		pcopy_free(&ctx->blocks[i].early);
		pcopy_free(&ctx->blocks[i].late);
	}
}

static void emit_assign(
		zend_op *opline, int from, int to, uint32_t lineno, const context *ctx) {
	ZEND_ASSERT(from >= 0 && to >= 0);
	ZEND_ASSERT(from != to);
	opline->opcode = ZEND_PHI_ASSIGN;
	opline->op1_type = to < ctx->new_num_vars ? IS_CV : IS_VAR;
	opline->op1.var = NUM_VAR(to);
	opline->op2_type = from < ctx->new_num_vars ? IS_CV : IS_VAR;
	opline->op2.var = NUM_VAR(from);
	opline->result_type = IS_UNUSED;
	opline->lineno = lineno;
}

static void pcopy_sequentialize(context *ctx, zend_op *opline, const pcopy *cpy, uint32_t lineno) {
	int i, *loc = ctx->loc, *pred = ctx->pred;
	zend_stack *ready = &ctx->stack;
	if (!cpy->num_elems) {
		return;
	}

	for (i = 0; i < cpy->num_elems; i++) {
		pcopy_elem *elem = &cpy->elems[i];
		emit_assign(opline++, elem->from, elem->to, lineno, ctx);
	}
	return;
	for (i = 0; i < cpy->num_elems; i++) {
		pcopy_elem *elem = &cpy->elems[i];
		loc[elem->from] = elem->from;
		pred[elem->to] = elem->from;
	}
	for (i = 0; i < cpy->num_elems; i++) {
		pcopy_elem *elem = &cpy->elems[i];
		if (loc[elem->to] < 0) {
			zend_stack_push(ready, &elem->to);
		}
	}
	//fprintf(stderr, "next\n");
	for (i = 0; i < cpy->num_elems; i++) {
		pcopy_elem *elem = &cpy->elems[i];
		while (!zend_stack_is_empty(&ctx->stack)) {
			int to = *(int *)zend_stack_top(ready);
			int from = pred[to], from_loc = loc[from];
			zend_stack_del_top(ready);
			//fprintf(stderr, "from %d (in %d) to %d\n", from, from_loc, to);

			emit_assign(opline++, from_loc, to, lineno, ctx);
			loc[from] = to;
			if (from == from_loc && pred[from] >= 0) {
				zend_stack_push(ready, &from);
			}
		}
		if (elem->to == loc[elem->to]) {
			/* Break cycle */
			fprintf(stderr, "cycle\n");
			continue;
			int extra_var = ctx->new_num_vars++;
			emit_assign(opline++, elem->to, extra_var, lineno, ctx);
			loc[elem->to] = extra_var;
			zend_stack_push(ready, &elem->to);
		}
	}
	for (i = 0; i < cpy->num_elems; i++) {
		loc[cpy->elems[i].from] = -1;
		pred[cpy->elems[i].to] = -1;
	}
}

static void collect_pcopies(context *ctx) {
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
					if (ctx->groups[phi->sources[i]].min != ctx->groups[phi->ssa_var].min) {
						pcopy_add_elem(
							&ctx->blocks[predecessors[i]].late,
							phi->sources[i], phi->ssa_var);
						OPT_STAT(tmp3)++;
					}
				}
			}
		} else {
			/* Ordinary, non-reference variables */
			for (i = 0; i < cfg->blocks[phi->block].predecessors_count; i++) {
				if (phi->sources[i] >= 0
						&& ctx->groups[phi->sources[i]].min != ctx->groups[phi->ssa_var].min) {
					pcopy_add_elem(
						&ctx->blocks[predecessors[i]].late,
						phi->sources[i], phi->ssa_var);
					OPT_STAT(tmp3)++;
				}
			}
		}
	} FOREACH_PHI_END();
}

#if DEBUG
static void debug_dump_pcopies(context *ctx) {
	int i, j;
	for (i = 0; i < ctx->ssa->cfg.blocks_count; i++) {
		const block_info *info = &ctx->blocks[i];
		if (info->early.num_elems) {
			fprintf(stderr, "BB%d early: ", i);
			for (j = 0; j < info->early.num_elems; j++) {
				const pcopy_elem *elem = &info->early.elems[j];
				fprintf(stderr, "%d->%d ", elem->from, elem->to);
			}
			fprintf(stderr, "\n");
		}
		if (info->late.num_elems) {
			fprintf(stderr, "BB%d late: ", i);
			for (j = 0; j < info->late.num_elems; j++) {
				const pcopy_elem *elem = &info->late.elems[j];
				fprintf(stderr, "%d->%d ", elem->from, elem->to);
			}
			fprintf(stderr, "\n");
		}
	}
}
#endif

static int count_groups(context *ctx) {
	int i, count = 0;
	for (i = 0; i < ctx->ssa->vars_count; i++) {
		if (ctx->groups[i].min == i) {
			count++;
		}
	}
	return count;
}

static inline void map_cv(zend_string **vars, uint32_t *new_this_var, const zend_op_array *op_array, int to, int from) {
	vars[to] = zend_string_copy(op_array->vars[from]);
	if (op_array->this_var == NUM_VAR(from)) {
		*new_this_var = NUM_VAR(to);
	}
}

static void assign_cvs_to_ssa_vars(context *ctx) {
	zend_ssa *ssa = ctx->ssa;
	zend_op_array *op_array = ctx->op_array;
	group *groups = ctx->groups;
	int num_groups = count_groups(ctx);
	int new_num_cvs = num_groups + ctx->num_extra_vars;
	zend_string **vars = emalloc(sizeof(zend_string *) * new_num_cvs);
	uint32_t new_this_var = (uint32_t) - 1;
	int i, j = 0;

	/* Assign argument CVs first, as they must stay the same */
	for (i = 0; i < op_array->num_args; i++) {
		map_cv(vars, &new_this_var, op_array, j, j);
		/*fprintf(stderr, "Alloc CV%d($%s) to SSA %d\n",
			j, ZSTR_VAL(vars[j]), op_array->last_var + i);*/
		groups[groups[op_array->last_var + i].min].cv = j++;
	}

	/* Assign CVs for all other groups */
	for (i = 0; i < ssa->vars_count; i++) {
		if (groups[i].min != i || groups[i].cv != -1) {
			continue;
		}

		map_cv(vars, &new_this_var, op_array, j, ssa->vars[i].var);
		//fprintf(stderr, "Alloc CV%d($%s) to SSA %d\n", j, ZSTR_VAL(vars[j]), i);
		groups[i].cv = j++;
	}

	// TODO Drop this, we don't really need to update SSA anymore
	for (i = 0; i < ssa->vars_count; i++) {
		zend_ssa_var *var;
		if (groups[i].min == -1) {
			continue;
		}

		var = &ssa->vars[i];
		var->var = groups[groups[i].min].cv;
	}

	ZEND_ASSERT(j == num_groups);
	for (; j < new_num_cvs; j++) {
		vars[j] = zend_long_to_str(j - num_groups);
	}

	for (i = 0; i < op_array->last_var; i++) {
		zend_string_release(op_array->vars[i]);
	}

	// TODO still used by assignment emission in pcopy sequentialization
	ctx->num_groups = num_groups;
	ctx->new_num_vars = new_num_cvs;
	ctx->tmp_var_offset = new_num_cvs - op_array->last_var;

	efree(op_array->vars);
	op_array->vars = vars;
	op_array->last_var = new_num_cvs;
	op_array->this_var = new_this_var;

	// TODO Make sure we never hit VAR phis
	// TODO new_num_vars unused
}

static inline int get_cv(const context *ctx, int ssa_var) {
	const zend_ssa *ssa = ctx->ssa;
	if (ssa_var < ssa->vars_count) {
		return ssa->vars[ssa_var].var;
	} else {
		return ssa_var - ssa->vars_count + ctx->num_groups;
	}
}

static inline void pcopy_to_cv(pcopy *cpy, const context *ctx) {
	int i;
	for (i = 0; i < cpy->num_elems; i++) {
		pcopy_elem *elem = &cpy->elems[i];
		elem->from = get_cv(ctx, elem->from);
		elem->to = get_cv(ctx, elem->to);
	}
}

static void convert_pcopies_to_cvs(context *ctx) {
	int i;
	for (i = 0; i < ctx->ssa->cfg.blocks_count; i++) {
		pcopy_to_cv(&ctx->blocks[i].early, ctx);
		pcopy_to_cv(&ctx->blocks[i].late, ctx);
	}
}

static inline zend_bool has_jump_instr(
		const zend_op_array *op_array, const zend_basic_block *block) {
	return block->successors[1] >= 0 || op_array->opcodes[block->end].opcode == ZEND_JMP;
}

static void compute_shiftlist(context *ctx) {
	const zend_op_array *op_array = ctx->op_array;
	const zend_cfg *cfg = &ctx->ssa->cfg;
	int i, j = 0, shift = 0;
	int *shiftlist = ctx->shiftlist;
	int *block_shiftlist = ctx->block_shiftlist;

	for (i = 0; i < cfg->blocks_count; i++) {
		const zend_basic_block *block = &cfg->blocks[i];
		zend_bool has_jump;

		*block_shiftlist++ = shift;

		if (!(block->flags & ZEND_BB_REACHABLE)) {
			continue;
		}

		while (j < block->start) {
			shiftlist[j++] = shift;
		}

		shift += pcopy_estimated_num_copies(&ctx->blocks[i].early);
		while (j < block->end) {
			shiftlist[j++] = shift;
		}

		has_jump = has_jump_instr(op_array, block);
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

#if DEBUG
	fprintf(stderr, "Shiftlist:\n");
	for (j = 0; j < op_array->last; j++) {
		fprintf(stderr, "%d: %d\n", j, shiftlist[j]);
	}
#endif

	ctx->new_num_opcodes = op_array->last + shift;
}

static inline int get_cv_for_op(const zend_ssa *ssa, int use, int def) {
	if (use >= 0) {
		ZEND_ASSERT(def < 0 || ssa->vars[def].var == ssa->vars[use].var);
		return NUM_VAR(ssa->vars[use].var);
	} else {
		ZEND_ASSERT(def >= 0);
		return NUM_VAR(ssa->vars[def].var);
	}
}

static inline void copy_instr(
		zend_op *new_opline, const zend_op *old_opline,
		const zend_op_array *op_array, const zend_op *new_opcodes,
		const int *block_shiftlist, const zend_ssa *ssa) {
#define TO_NEW(block_num) \
		(new_opcodes + cfg->blocks[block_num].start + block_shiftlist[block_num])
	const zend_cfg *cfg = &ssa->cfg;
	zend_basic_block *block = &cfg->blocks[cfg->map[old_opline - op_array->opcodes]];
	const zend_ssa_op *ssa_op = &ssa->ops[old_opline - op_array->opcodes];
	*new_opline = *old_opline;

	/* Adjust CV offsets */
	if (new_opline->op1_type == IS_CV) {
		new_opline->op1.var = get_cv_for_op(ssa, ssa_op->op1_use, ssa_op->op1_def);
	}
	if (new_opline->op2_type == IS_CV) {
		new_opline->op2.var = get_cv_for_op(ssa, ssa_op->op2_use, ssa_op->op2_def);
	}
	if (new_opline->result_type == IS_CV) {
		new_opline->result.var = get_cv_for_op(ssa, ssa_op->result_use, ssa_op->result_def);
	}

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
		case ZEND_NEW:
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

static void insert_copies(context *ctx) {
	zend_op_array *op_array = ctx->op_array;
	const zend_ssa *ssa = ctx->ssa;
	const zend_cfg *cfg = &ssa->cfg;
	const zend_op *old = op_array->opcodes;
	const int *block_shiftlist = ctx->block_shiftlist;
	zend_op *new_opcodes = ecalloc(sizeof(zend_op), ctx->new_num_opcodes);
	zend_op *new = new_opcodes;
	int i;
	for (i = 0; i < cfg->blocks_count; i++) {
		const zend_basic_block *block = &cfg->blocks[i];
		zend_bool has_jump;
		if (!(block->flags & ZEND_BB_REACHABLE)) {
			continue;
		}

		while (old < &op_array->opcodes[block->start]) {
			copy_instr(new++, old++, op_array, new_opcodes, block_shiftlist, ssa);
		}

		pcopy_sequentialize(ctx, new, &ctx->blocks[i].early,
			op_array->opcodes[block->start].lineno);
		new += pcopy_estimated_num_copies(&ctx->blocks[i].early);

		while (old < &op_array->opcodes[block->end]) {
			copy_instr(new++, old++, op_array, new_opcodes, block_shiftlist, ssa);
		}

		// TODO This is pretty ugly
		has_jump = has_jump_instr(op_array, block);
		if (has_jump) {
			pcopy_sequentialize(ctx, new, &ctx->blocks[i].late,
				op_array->opcodes[block->end].lineno);
			new += pcopy_estimated_num_copies(&ctx->blocks[i].late);
			copy_instr(new++, old++, op_array, new_opcodes, block_shiftlist, ssa);
		} else {
			copy_instr(new++, old++, op_array, new_opcodes, block_shiftlist, ssa);
			pcopy_sequentialize(ctx, new, &ctx->blocks[i].late,
				op_array->opcodes[block->end].lineno);
			new += pcopy_estimated_num_copies(&ctx->blocks[i].late);
		}
	}

	while (old < &op_array->opcodes[op_array->last]) {
		copy_instr(new++, old++, op_array, new_opcodes, block_shiftlist, ssa);
	}

	efree(op_array->opcodes);
	op_array->opcodes = new_opcodes;
	op_array->last = ctx->new_num_opcodes;
}

static inline void adjust_var_offsets_in_opline(zend_op *opline, uint32_t tmp_var_offset) {
	if (opline->op1_type & (IS_TMP_VAR|IS_VAR)) {
		opline->op1.var += tmp_var_offset * sizeof(zval);
	}
	if (opline->op2_type & (IS_TMP_VAR|IS_VAR)) {
		opline->op2.var += tmp_var_offset * sizeof(zval);
	}
	if (opline->result_type & (IS_TMP_VAR|IS_VAR)) {
		opline->result.var += tmp_var_offset * sizeof(zval);
	}
}

static void adjust_var_offsets(context *ctx) {
	zend_op_array *op_array = ctx->op_array;
	int i, tmp_var_offset = ctx->tmp_var_offset;
	for (i = 0; i < op_array->last; i++) {
		adjust_var_offsets_in_opline(&op_array->opcodes[i], tmp_var_offset);
	}
}

static void adjust_auxiliary_structures(context *ctx) {
	zend_op_array *op_array = ctx->op_array;
	int i, *shiftlist = ctx->shiftlist;
	uint32_t tmp_var_offset = ctx->tmp_var_offset;

	/* Update live ranges */
	if (op_array->last_live_range) {
		for (i = 0; i < op_array->last_live_range; i++) {
			zend_live_range *range = &op_array->live_range[i];
			range->var += tmp_var_offset * sizeof(zval);
			range->start += shiftlist[range->start];
			range->end += shiftlist[range->end];
		}
	}

	/* Update try/catch/finally offsets */
	if (op_array->last_try_catch) {
		for (i = 0; i < op_array->last_try_catch; i++) {
			zend_try_catch_element *try_catch = &op_array->try_catch_array[i];
			try_catch->try_op += shiftlist[try_catch->try_op];
			try_catch->catch_op += shiftlist[try_catch->catch_op];
			if (try_catch->finally_op) {
				try_catch->finally_op += shiftlist[try_catch->finally_op];
				try_catch->finally_end += shiftlist[try_catch->finally_end];
			}
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

static void adjust_cfg(context *ctx) {
	zend_cfg *cfg = &ctx->ssa->cfg;
	int i, shift = 0;
	for (i = 0; i < cfg->blocks_count; i++) {
		zend_basic_block *block = &cfg->blocks[i];
		block_info *info = &ctx->blocks[i];
		if (!(block->flags & ZEND_BB_REACHABLE)) {
			continue;
		}

		block->start += shift;
		shift += pcopy_estimated_num_copies(&info->early);
		shift += pcopy_estimated_num_copies(&info->late);
		block->end += shift;
	}
}

static void insert_pcopies(context *ctx) {
	zend_ssa *ssa = ctx->ssa;
	zend_op_array *op_array = ctx->op_array;

	ctx->predom = zend_arena_alloc(&ctx->arena, sizeof(int) * ssa->vars_count);
	compute_dominance_preorder(ctx);

	ctx->groups = zend_arena_alloc(&ctx->arena, sizeof(group) * ssa->vars_count);
	coalesce_vars(ctx);

	ctx->blocks = zend_arena_alloc(&ctx->arena, sizeof(block_info) * ssa->cfg.blocks_count);
	init_block_pcopies(ctx);

	ctx->new_num_temps = op_array->T;
	ctx->num_extra_vars = 0;
	collect_pcopies(ctx);

#if DEBUG
	debug_dump_pcopies(ctx);
#endif

	ctx->new_num_vars = 0;
	assign_cvs_to_ssa_vars(ctx);
	convert_pcopies_to_cvs(ctx);

#if DEBUG
	debug_dump_pcopies(ctx);
#endif

	ctx->shiftlist = zend_arena_alloc(&ctx->arena, sizeof(int) * op_array->last);
	ctx->block_shiftlist = zend_arena_alloc(&ctx->arena, sizeof(int) * ssa->cfg.blocks_count);
	compute_shiftlist(ctx);

	ctx->loc = zend_arena_alloc(&ctx->arena,
		sizeof(int) * (ctx->new_num_vars + ctx->new_num_temps));
	ctx->pred = zend_arena_alloc(&ctx->arena,
		sizeof(int) * (ctx->new_num_vars + ctx->new_num_temps));
	memset(ctx->loc, -1, sizeof(int) * (ctx->new_num_vars + ctx->new_num_temps));
	memset(ctx->pred, -1, sizeof(int) * (ctx->new_num_vars + ctx->new_num_temps));
	insert_copies(ctx);
	zend_arena_release(&ctx->arena, ctx->loc);

	adjust_var_offsets(ctx);
	adjust_auxiliary_structures(ctx);
	adjust_cfg(ctx);

	op_array->T = ctx->new_num_temps;

	free_block_pcopies(ctx);
}

static void ssa_destroy(ssa_opt_ctx *ssa_ctx) {
	context ctx;
	ctx.arena = ssa_ctx->opt_ctx->arena;
	ctx.op_array = ssa_ctx->op_array;
	ctx.ssa = ssa_ctx->ssa;
	ctx.info = ssa_ctx->cfg_info;
	ctx.liveness = ssa_ctx->liveness;
	zend_stack_init(&ctx.stack, sizeof(int));

	remove_essa_pis(ctx.ssa);
	insert_pcopies(&ctx);

	zend_stack_destroy(&ctx.stack);
	ssa_ctx->opt_ctx->arena = ctx.arena;
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

static inline int num_predecessors(const zend_cfg *cfg, const zend_basic_block *block) {
	int j, count = 0, *predecessors = &cfg->predecessors[block->predecessor_offset];
	for (j = 0; j < block->predecessors_count; j++) {
		if (predecessors[j] >= 0) {
			count++;
		}
	}
	return count;
}

static void count_critical_edges(ssa_opt_ctx *ctx) {
	const zend_cfg *cfg = &ctx->ssa->cfg;
	int i;
	for (i = 0; i < cfg->blocks_count; i++) {
		zend_basic_block *block = &cfg->blocks[i];
		if (!(block->flags & ZEND_BB_REACHABLE)) {
			continue;
		}

		if (block->successors[1] < 0) {
			/* Successor edge can't be critical if there's only one */
			if (block->successors[0] >= 0) {
				OPT_STAT(tmp2)++;
			}
			continue;
		}

		if (num_predecessors(cfg, &cfg->blocks[block->successors[0]]) != 1) {
			OPT_STAT(tmp)++;
		} else {
			OPT_STAT(tmp2)++;
		}
		if (num_predecessors(cfg, &cfg->blocks[block->successors[1]]) != 1) {
			OPT_STAT(tmp)++;
		} else {
			OPT_STAT(tmp2)++;
		}
	}
}

void ssa_optimize_destroy_ssa(ssa_opt_ctx *ctx) {
	check_interferences(ctx); 
	count_critical_edges(ctx);
	ssa_destroy(ctx);
}
