#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa/helpers.h"
#include "Optimizer/ssa/instructions.h"

/* A "simple" type is a type that never uses refcounting */
#define MAY_BE_SIMPLE (MAY_BE_NULL|MAY_BE_TRUE|MAY_BE_FALSE|MAY_BE_LONG|MAY_BE_DOUBLE)
#define IS_SIMPLE(t) (((t) & MAY_BE_ANY) == ((t) & MAY_BE_SIMPLE))

static inline zend_bool is_compound_assign(zend_op *opline) {
	switch (opline->opcode) {
		case ZEND_ASSIGN_ADD:
		case ZEND_ASSIGN_SUB:
		case ZEND_ASSIGN_MUL:
		case ZEND_ASSIGN_DIV:
		case ZEND_ASSIGN_MOD:
		case ZEND_ASSIGN_SL:
		case ZEND_ASSIGN_SR:
		case ZEND_ASSIGN_CONCAT:
		case ZEND_ASSIGN_BW_OR:
		case ZEND_ASSIGN_BW_AND:
		case ZEND_ASSIGN_BW_XOR:
		case ZEND_ASSIGN_POW:
			return 1;
		default:
			return 0;
	}
}

static inline zend_bool is_incdec(zend_op *opline) {
	switch (opline->opcode) {
		case ZEND_PRE_INC:
		case ZEND_PRE_DEC:
		case ZEND_POST_INC:
		case ZEND_POST_DEC:
			return 1;
		default:
			return 0;
	}
}

static inline zend_bool can_tmpvar_op1(zend_op *opline) {
	switch (opline->opcode) {
		case ZEND_ASSIGN_ADD:
		case ZEND_ASSIGN_SUB:
		case ZEND_ASSIGN_MUL:
		case ZEND_ASSIGN_DIV:
		case ZEND_ASSIGN_MOD:
		case ZEND_ASSIGN_SL:
		case ZEND_ASSIGN_SR:
		case ZEND_ASSIGN_CONCAT:
		case ZEND_ASSIGN_BW_OR:
		case ZEND_ASSIGN_BW_AND:
		case ZEND_ASSIGN_BW_XOR:
		case ZEND_ASSIGN_POW:
			/* If ev!=0 it is a DIM/OBJ compound assign */
			return opline->extended_value == 0;
		case ZEND_ISSET_ISEMPTY_VAR:
			/* Quick set gives special meaning to CV */
			return !(opline->extended_value & ZEND_QUICK_SET);
		case ZEND_ASSIGN_REF:
		case ZEND_ASSIGN_DIM:
		case ZEND_ASSIGN_OBJ:
		case ZEND_PRE_INC_OBJ:
		case ZEND_PRE_DEC_OBJ:
		case ZEND_POST_INC_OBJ:
		case ZEND_POST_DEC_OBJ:
		case ZEND_FETCH_DIM_W:
		case ZEND_FETCH_DIM_RW:
		case ZEND_FETCH_DIM_UNSET:
		case ZEND_FETCH_DIM_FUNC_ARG: // TODO: Maybe FUNC_ARG is okay?
		case ZEND_FETCH_OBJ_W:
		case ZEND_FETCH_OBJ_RW:
		case ZEND_FETCH_OBJ_UNSET:
		case ZEND_FETCH_OBJ_FUNC_ARG:
		case ZEND_UNSET_DIM:
		case ZEND_UNSET_OBJ:
		case ZEND_SEND_REF:
		case ZEND_SEND_VAR_EX:
		case ZEND_SEND_VAR_NO_REF:
		case ZEND_SEND_UNPACK:
		case ZEND_SEND_ARRAY:
		case ZEND_SEND_USER:
		case ZEND_RETURN_BY_REF:
		case ZEND_BIND_GLOBAL:
			return 0;
	}
	return 1;
}

static inline zend_bool can_tmpvar_op2(zend_op *opline) {
	switch (opline->opcode) {
		case ZEND_ASSIGN_REF:
		case ZEND_CATCH:
		case ZEND_BIND_LEXICAL:
			return 0;
	}

	return 1;
}

typedef struct {
	int min;
	int next;
} alias_ssa;

typedef struct {
	zend_op_array *op_array;
	zend_ssa *ssa;
	alias_ssa *alias;
} context;

#define CANNOT_ALIAS -1

static inline zend_bool is_alias(context *ctx, int var1, int var2) {
	int i = ctx->alias[var1].min;
	for (; i != -1; i = ctx->alias[i].next) {
		if (i == var2) {
			return 1;
		}
	}
	return 0;
}

static inline void swap(int *a, int *b) {
	int t = *a;
	*a = *b;
	*b = t;
}

static void mark_unaliasable(context *ctx, int var) {
	int i = ctx->alias[var].min;
	if (i == CANNOT_ALIAS) {
		return;
	}
	while (i != -1) {
		ctx->alias[i].min = CANNOT_ALIAS;
		i = ctx->alias[i].next;
	}
}

static inline void merge_lists(context *ctx, int var1, int var2) {
	//php_printf("Merging %d and %d\n", var1, var2);
	int next = ctx->alias[var1].next;
	while (next != -1 && var2 != -1) {
		if (next > var2) {
			ctx->alias[var1].next = var2;
			var1 = var2;
			var2 = next;
		} else {
			var1 = next;
		}
		next = ctx->alias[var1].next;
	}
	if (next == -1) {
		ctx->alias[var1].next = var2;
	}
}

static void mark_aliases(context *ctx, int var1, int var2) {
	int i;

	ZEND_ASSERT(ctx->alias[var1].min != CANNOT_ALIAS);
	ZEND_ASSERT(ctx->alias[var2].min != CANNOT_ALIAS);
	var1 = ctx->alias[var1].min;
	var2 = ctx->alias[var2].min;
	if (var1 == var2) {
		return;
	}
	if (var1 > var2) {
		swap(&var1, &var2);
	}

	for (i = var2; i != -1; i = ctx->alias[i].next) {
		ctx->alias[i].min = var1;
	}
	merge_lists(ctx, var1, var2);
}

static void must_alias(context *ctx, int var1, int var2) {
	ZEND_ASSERT(var2 >= 0);
	if (ctx->alias[var1].min == CANNOT_ALIAS) {
		mark_unaliasable(ctx, var2);
		return;
	}
	if (ctx->alias[var2].min == CANNOT_ALIAS) {
		mark_unaliasable(ctx, var1);
		return;
	}

	mark_aliases(ctx, var1, var2);
}

static void may_alias(context *ctx, int var1, int var2) {
	ZEND_ASSERT(var2 >= 0);
	if (ctx->alias[var1].min == CANNOT_ALIAS) {
		return;
	}
	if (ctx->alias[var2].min == CANNOT_ALIAS) {
		return;
	}

	mark_aliases(ctx, var1, var2);
}

static void debug_dump(context *ctx) {
	int j;
	for (j = 0; j < ctx->ssa->vars_count; ++j) {
		if (ctx->alias[j].min == j) {
			int k = j;
			do {
				php_printf("%d ", k);
				k = ctx->alias[k].next;
			} while (k != -1);
			php_printf("\n");
		}
	}
	php_printf("Unaliased: ");
	for (j = 0; j < ctx->ssa->vars_count; ++j) {
		if (ctx->alias[j].min == -1) {
			php_printf("%d ", j);
		}
	}
	php_printf("\n");
}

static void find_aliases(context *ctx) {
	zend_ssa *ssa = ctx->ssa;
	zend_op_array *op_array = ctx->op_array;
	zend_ssa_phi *phi;
	int i;

	/* Pass 1: Mark variables as definitely not aliasable, and alias variables that
	 * must be aliases. The "maybe" cases are handled in pass 2. */
	for (i = 0; i < op_array->last; i++) {
		zend_ssa_op *ssa_op = &ssa->ops[i];
		zend_op *opline = &op_array->opcodes[i];

		/* Discard variables that are used as operands not supporting TMPs. */
		if (ssa_op->op1_use >= 0 && !can_tmpvar_op1(opline)) {
			mark_unaliasable(ctx, ssa_op->op1_use);
		}
		if (ssa_op->op2_use >= 0 && !can_tmpvar_op2(opline)) {
			mark_unaliasable(ctx, ssa_op->op2_use);
		}

		if (ssa_op->op1_use >= 0 && ssa_op->op1_def >= 0) {
			/* Normal assignments don't have to alias (we can split the alias set at the
			 * assignment, so these are handled in the next pass. Compound assignments must
			 * alias, as splitting them would require emitting the op and an ASSIGN. */
			if (opline->opcode != ZEND_ASSIGN) {
				must_alias(ctx, ssa_op->op1_use, ssa_op->op1_def);
			}

			/* This is the case where the result of a (simple/compound) assignment is used.
			 * To handle it, we would have to split (or mark unaliasable) at the next
			 * assignment, if the result is used after it. For now it's not worth the effort. */
			// TODO Implement the aforementioned
			if (ssa_op->result_def >= 0) {
				/* POST_INC/POST_DEC return a TMP (rather than a VAR), so it cannot be marked
				 * UNUSED. Instead we explicitly check it isn't used (the FREE that used it
				 * has already been dropped in an earlier pass). */
				if ((opline->opcode != ZEND_POST_INC && opline->opcode != ZEND_POST_DEC)
						|| var_used(&ssa->vars[ssa_op->result_def])
				) {
					mark_unaliasable(ctx, ssa_op->op1_def);
				}
			}
		}
		if (ssa_op->op2_use >= 0 && ssa_op->op2_def >= 0) {
			must_alias(ctx, ssa_op->op2_use, ssa_op->op2_def);
		}

		/* Arguments are tightly bound to CVs */
		if (ssa_op->result_def >= 0
				&& (opline->opcode == ZEND_RECV || opline->opcode == ZEND_RECV_INIT)) {
			mark_unaliasable(ctx, ssa_op->result_def);
		}
	}

	/* Sources and result of Phi nodes must be aliased. */
	FOREACH_PHI(phi) {
		if (!ssa->vars[phi->ssa_var].no_val) {
			int source;
			FOREACH_PHI_SOURCE(phi, source) {
				must_alias(ctx, phi->ssa_var, source);
			} FOREACH_PHI_SOURCE_END();
		}
	} FOREACH_PHI_END();

	//debug_dump(ctx);

	/* Pass 2: Mark variables that may be aliases if it does not require
	 * propagating a CANNOT_ALIAS status. */
	for (i = 0; i < ssa->vars_count; i++) {
		zend_ssa_var *var = &ssa->vars[i];

		if (var->no_val || ctx->alias[i].min == CANNOT_ALIAS) {
			continue;
		}

		//debug_dump(ctx);

		if (var->definition >= 0) {
			zend_op *opline = &op_array->opcodes[var->definition];
			zend_ssa_op *ssa_op = &ssa->ops[var->definition];
			if (ssa_op->op1_def == i && opline->opcode == ZEND_ASSIGN) {
				/* We can reuse the LHS of an assignment */
				may_alias(ctx, i, ssa_op->op1_use);

				if (ssa_op->op2_use >= 0) {
					if (opline->op2_type == IS_CV) {
						/* We can alias both variables in an $x = $y assignment, if $y is not used
						 * after this assignment and if $x is not used after any least recent
						 * assignment to $y. */
						/*zend_ssa_var *use_var = &ssa->vars[ssa_op->op2_def];
						if (!var_used(use_var)) {
							may_alias(ctx, i, ssa_op->op2_def);
						}*/
						// TODO This should be handled in a copy propagation pass, I think
					} else {
						/* We can alias the LHS to a temporary RHS, as we know it is only
						 * used once (in this assignment). */
						may_alias(ctx, i, ssa_op->op2_use);
					}
				}
			}
		}
	}

	//debug_dump(ctx);
}

static int find_tmpvar(context *ctx, int i) {
	for (; i != -1; i = ctx->alias[i].next) {
		zend_ssa_var *var = &ctx->ssa->vars[i];
		if (var->definition >= 0) {
			zend_ssa_op *ssa_op = &ctx->ssa->ops[var->definition];
			if (ssa_op->result_def == i) {
				zend_op *opline = &ctx->op_array->opcodes[var->definition];
				if (opline->result_type == IS_TMP_VAR) {
					return opline->result.var;
				}
			}
		}
	}
	return -1;
}

/* Either drops ASSIGN or converts it to QM_ASSIGN */
static void adjust_assign(
	context *ctx, zend_ssa_op *ssa_op, zend_op *opline, zend_ssa_var *var, int tmpvar
) {
	zend_ssa *ssa = ctx->ssa;
	if (ssa_op->op2_use >= 0 && is_alias(ctx, ssa_op->op1_def, ssa_op->op2_use)) {
		/* Rename assigned-to to assigned-from var */
		rename_var_uses(ssa, ssa_op->op1_def, ssa_op->op2_use);
		ssa_op->op1_def = -1;

		/* Drop ASSIGN */
		remove_instr(ssa, opline, ssa_op);
	} else {
		/* Replace ASSIGN by QM_ASSIGN */
		opline->opcode = ZEND_QM_ASSIGN;
		opline->result_type = IS_TMP_VAR;
		opline->result.var = tmpvar;
		COPY_NODE(opline->op1, opline->op2);
		opline->op2_type = IS_UNUSED;

		/* Result now defines the var */
		ssa_op->result_def = ssa_op->op1_def;
		ssa_op->op1_def = -1;

		/* Remove spurious SSA use */
		remove_op1_use(ssa, ssa_op);
		ssa_op->op1_use = ssa_op->op2_use;
	}
}

/* Converts ASSIGN_OP to OP */
static void adjust_compound_assign(
	context *ctx, zend_ssa_op *ssa_op, zend_op *opline, zend_ssa_var *var, int tmpvar
) {
	opline->opcode = instr_get_compound_assign_op(opline);
	COPY_NODE(opline->result, opline->op1);

	if (ssa_op->result_def >= 0) {
		rename_var_uses(ctx->ssa, ssa_op->result_def, ssa_op->op1_def);
	}
	ssa_op->result_def = ssa_op->op1_def;
	ssa_op->op1_def = -1;
}

/* Converts (PRE|POST)_(INC|DEC) to ADD|SUB 1 */
static void adjust_incdec(
	context *ctx, zend_ssa_op *ssa_op, zend_op *opline, zend_ssa_var *var, int tmpvar
) {
	zval one;
	ZVAL_LONG(&one, 1);

	opline->opcode = opline->opcode == ZEND_PRE_INC || opline->opcode == ZEND_POST_INC
		? ZEND_ADD : ZEND_SUB;
	COPY_NODE(opline->result, opline->op1);

	opline->op2_type = IS_CONST;
	opline->op2.constant = zend_optimizer_add_literal(ctx->op_array, &one);

	if (ssa_op->result_def >= 0) {
		rename_var_uses(ctx->ssa, ssa_op->result_def, ssa_op->op1_def);
	}
	ssa_op->result_def = ssa_op->op1_def;
	ssa_op->op1_def = -1;
}

static void replace_tmpvars(context *ctx) {
	zend_ssa *ssa = ctx->ssa;
	int i;
	for (i = 0; i < ssa->vars_count; i++) {
		int j, tmpvar;
		if (ctx->alias[i].min != i) {
			/* Not aliasable or not start of chain */
			continue;
		}
		if (ctx->alias[i].next == -1) {
			/* Only one variable in alias set -- not useful */
			continue;
		}

		tmpvar = find_tmpvar(ctx, i);
		if (tmpvar == -1) {
			// TODO Adjust stack size!!! For now running with optimization 12
			uint32_t n = ctx->op_array->T++;
			tmpvar = (zend_intptr_t) ZEND_CALL_VAR_NUM(NULL, ctx->op_array->last_var + n);
		}

		/* Pass 1: Replace uses */
		for (j = i; j != -1; j = ctx->alias[j].next) {
			zend_ssa_var *var = &ssa->vars[j];
			int use;
			FOREACH_USE(var, use) {
				zend_op *opline = &ctx->op_array->opcodes[use];
				zend_ssa_op *ssa_op = &ssa->ops[use];
				if (ssa_op->op1_use == j && opline->opcode != ZEND_ASSIGN) {
					opline->op1_type = IS_TMP_VAR;
					opline->op1.var = tmpvar;

					if (opline->opcode == ZEND_SEND_VAR) {
						opline->opcode = ZEND_SEND_VAL;
					}
				}
				if (ssa_op->op2_use == j) {
					opline->op2_type = IS_TMP_VAR;
					opline->op2.var = tmpvar;
				}
			} FOREACH_USE_END();
		}

		/* Pass 2: Replace definitions. This is a separate pass, because we may be adjusting
		 * use chains in here and don't want to skip over anything for that reason. */
		for (j = i; j != -1; j = ctx->alias[j].next) {
			zend_ssa_var *var = &ssa->vars[j];
			var->var = tmpvar;
			if (var->definition >= 0) {
				zend_op *opline = &ctx->op_array->opcodes[var->definition];
				zend_ssa_op *ssa_op = &ssa->ops[var->definition];
				if (ssa_op->result_def == j) {
					opline->result_type = IS_TMP_VAR;
					opline->result.var = tmpvar;
				}
				if (ssa_op->op1_def == j) {
					if (opline->opcode == ZEND_ASSIGN) {
						adjust_assign(ctx, ssa_op, opline, var, tmpvar);
					} else if (is_incdec(opline)) {
						adjust_incdec(ctx, ssa_op, opline, var, tmpvar);
					} else if (is_compound_assign(opline)) {
						adjust_compound_assign(ctx, ssa_op, opline, var, tmpvar);
					}
				}
			}
		}
	}
}

static void init_alias_ssa(context *ctx) {
	int i;
	for (i = 0; i < ctx->ssa->vars_count; i++) {
		zend_ssa_var_info *var_info = &ctx->ssa->var_info[i];

		ctx->alias[i].next = -1;
		if (IS_SIMPLE(var_info->type) && !(var_info->type & MAY_BE_UNDEF)) {
			/* Initialize aliases to identity mapping */
			ctx->alias[i].min = i;
		} else {
			/* Non-simple variables cannot be turned into TMPs, so we exclude
			 * them from this analysis right away. */
			ctx->alias[i].min = CANNOT_ALIAS;
		}
	}
}

/* Removes FREEs on temporaries that hold simple values. We remove them first for easier
 * handling of POST_INC/DEC, though it's obviously also useful outside of that. */
static void remove_redundant_frees(zend_ssa *ssa, zend_op_array *op_array) {
	int i;
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		if (opline->opcode == ZEND_FREE) {
			zend_ssa_op *ssa_op = &ssa->ops[i];
			zend_ssa_var_info *var_info = &ssa->var_info[ssa_op->op1_use];
			if (IS_SIMPLE(var_info->type)) {
				remove_instr(ssa, opline, ssa_op);
			}
		}
	}
}

static void foo(zend_op_array *op_array, zend_ssa *ssa) {
	context ctx;

	ctx.op_array = op_array;
	ctx.ssa = ssa;
	ctx.alias = alloca(sizeof(alias_ssa) * ssa->vars_count);

	remove_redundant_frees(ssa, op_array);
	init_alias_ssa(&ctx);
	find_aliases(&ctx);
	replace_tmpvars(&ctx);
}

#if 0
/* This turns CV lookups into TMP lookups on the *same* variable, if the CV
 * is known to be defined and simple. This saves the undef and deref checks
 * at runtime. Also turns ASSIGN into QM_ASSIGN if possible. */
static void mark_cv_uses_as_tmp(zend_op_array *op_array) {
	zend_op *opline = op_array->opcodes;
	zend_op *end = opline + op_array->last;
	while (opline != end) {
		uint32_t t1 = OP1_INFO();
		uint32_t t2 = OP2_INFO();
		zend_bool op1_simple = (t1 & MAY_BE_ANY) == (t1 & MAY_BE_SIMPLE);
		zend_bool op2_simple = (t2 & MAY_BE_ANY) == (t2 & MAY_BE_SIMPLE);

		if (opline->opcode == ZEND_ASSIGN) {
			if (opline->op1_type == IS_CV && op1_simple && op2_simple) {
				opline->opcode = ZEND_QM_ASSIGN;
				COPY_NODE(opline->result, opline->op1);
				COPY_NODE(opline->op1, opline->op2);
				opline->op2_type = IS_UNUSED;
				opline->result_type = IS_TMP_VAR;
			}
		}

		if (opline->op1_type == IS_CV && can_tmpvar_op1(opline)) {
			if (op1_simple && !(t1 & MAY_BE_UNDEF)) {
				opline->op1_type = IS_TMP_VAR;
			}
		}
		if (opline->op2_type == IS_CV && can_tmpvar_op2(opline)) {
			if (op2_simple && !(t2 & MAY_BE_UNDEF)) {
				opline->op2_type = IS_TMP_VAR;
			}
		}
		opline++;
	}
}
#endif

void ssa_optimize_cv_to_tmp(zend_optimizer_ctx *ctx, zend_op_array *op_array, zend_ssa *ssa) {
	foo(op_array, ssa);
	//mark_cv_uses_as_tmp(op_array);
}
