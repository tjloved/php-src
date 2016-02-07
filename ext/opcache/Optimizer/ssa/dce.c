#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/instructions.h"
#include "Optimizer/statistics.h"

typedef struct {
	zend_ssa *ssa;
	zend_op_array *op_array;
	zend_bitset instr_dead;
	zend_bitset phi_dead;
	zend_bitset instr_worklist;
	zend_bitset phi_worklist;
	uint32_t instr_worklist_len;
	uint32_t phi_worklist_len;
} context;

#define MAY_HAVE_DTOR \
	(MAY_BE_OBJECT|MAY_BE_RESOURCE \
	|MAY_BE_ARRAY_OF_ARRAY|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_ARRAY_OF_RESOURCE)

static inline zend_bool may_have_side_effects(
		zend_op_array *op_array, zend_ssa *ssa, zend_op *opline, zend_ssa_op *ssa_op) {
	if (may_throw(op_array, ssa, opline, ssa_op)) {
		return 1;
	}

	switch (opline->opcode) {
		case ZEND_JMP:
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
		case ZEND_JMPZNZ:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_ASSERT_CHECK:
			/* For our purposes a jump is a side effect. */
			return 1;
		case ZEND_BEGIN_SILENCE:
		case ZEND_END_SILENCE:
		case ZEND_ECHO:
		case ZEND_INCLUDE_OR_EVAL:
		case ZEND_THROW:
		case ZEND_EXT_STMT:
		case ZEND_EXT_FCALL_BEGIN:
		case ZEND_EXT_FCALL_END:
		case ZEND_EXT_NOP:
		case ZEND_TICKS:
			/* Intrinsic side effects */
			return 1;
		case ZEND_DO_FCALL:
		case ZEND_DO_FCALL_BY_NAME:
		case ZEND_DO_ICALL:
		case ZEND_DO_UCALL:
			/* For now assume all calls have side effects */
			return 1;
		case ZEND_ASSIGN_REF:
			return 1;
		case ZEND_ASSIGN:
		{
			uint32_t t1 = OP1_INFO();
			uint32_t t2 = OP2_INFO();
			if (t1 & MAY_BE_REF) {
				return 1;
			}
			if (t1 & MAY_HAVE_DTOR) {
				/* DCE might delay call to dtor */
				return 1;
			}
			if (t2 & MAY_HAVE_DTOR) {
				/* DCE might result in dtor firing too early */
				return 1;
			}
			return 0;
		}
		case ZEND_UNSET_VAR:
			if (opline->extended_value & ZEND_QUICK_SET) {
				return (OP1_INFO() & (MAY_BE_REF|MAY_HAVE_DTOR)) != 0;
			}
			return 0;
		case ZEND_PRE_INC:
		case ZEND_POST_INC:
		case ZEND_PRE_DEC:
		case ZEND_POST_DEC:
			/* If there's no op1 def it's an indirected incref not tracked by SSA */
			return (OP1_INFO() & MAY_BE_REF) || ssa_op->op1_def < 0;
	}
	return 0;
}

static inline zend_bool has_improper_op1_use(zend_op *opline) {
	return opline->opcode == ZEND_ASSIGN
		|| (opline->opcode == ZEND_UNSET_VAR && opline->extended_value & ZEND_QUICK_SET);
}

static inline void add_to_worklists(context *ctx, int var_num) {
	zend_ssa_var *var = &ctx->ssa->vars[var_num];
	if (var->definition >= 0) {
		if (zend_bitset_in(ctx->instr_dead, var->definition)) {
			zend_bitset_incl(ctx->instr_worklist, var->definition);
		}
	} else if (var->definition_phi) {
		if (zend_bitset_in(ctx->phi_dead, var_num)) {
			zend_bitset_incl(ctx->phi_worklist, var_num);
		}
	}
}

static inline void add_to_phi_worklist_only(context *ctx, int var_num) {
	zend_ssa_var *var = &ctx->ssa->vars[var_num];
	if (var->definition_phi && zend_bitset_in(ctx->phi_dead, var_num)) {
		zend_bitset_incl(ctx->phi_worklist, var_num);
	}
}

static inline void add_operands_to_worklists(context *ctx, zend_op *opline, zend_ssa_op *ssa_op) {
	if (ssa_op->result_use >= 0) {
		add_to_worklists(ctx, ssa_op->result_use);
	}
	if (ssa_op->op1_use >= 0 && !has_improper_op1_use(opline)) {
		add_to_worklists(ctx, ssa_op->op1_use);
	}
	if (ssa_op->op2_use >= 0) {
		add_to_worklists(ctx, ssa_op->op2_use);
	}
}

static inline void add_phi_sources_to_worklists(context *ctx, zend_ssa_phi *phi) {
	zend_ssa *ssa = ctx->ssa;
	int source;
	FOREACH_PHI_SOURCE(phi, source) {
		add_to_worklists(ctx, source);
	} FOREACH_PHI_SOURCE_END();
}

static inline zend_bool is_var_dead(context *ctx, int var_num) {
	zend_ssa_var *var = &ctx->ssa->vars[var_num];
	if (var->definition_phi) {
		return zend_bitset_in(ctx->phi_dead, var_num);
	} else if (var->definition >= 0) {
		return zend_bitset_in(ctx->instr_dead, var->definition);
	} else {
		/* This means the definition was already removed, so it's also dead */
		return 1;
	}
}

static void dce_instr(context *ctx, zend_op *opline, zend_ssa_op *ssa_op) {
	zend_ssa *ssa = ctx->ssa;
	int free_var = -1;
	zend_uchar free_var_type;

	if (opline->opcode == ZEND_NOP) {
		return;
	}

	/* We mark FREEs as dead, but they're only really dead if the destroyed var is dead */
	if (opline->opcode == ZEND_FREE && !is_var_dead(ctx, ssa_op->op1_use)) {
		return;
	}

	OPT_STAT(dce_dead_instr)++;

	// TODO Two free vars?
	if ((opline->op1_type & (IS_VAR|IS_TMP_VAR)) && !is_var_dead(ctx, ssa_op->op1_use)) {
		free_var = ssa_op->op1_use;
		free_var_type = opline->op1_type;
	} else if ((opline->op2_type & (IS_VAR|IS_TMP_VAR)) && !is_var_dead(ctx, ssa_op->op2_use)) {
		free_var = ssa_op->op2_use;
		free_var_type = opline->op2_type;
	}

	rename_defs_of_instr(ctx->ssa, ssa_op);
	remove_instr(ctx->ssa, opline, ssa_op);

	if (free_var >= 0) {
		// TODO Sometimes we can mark the var as EXT_UNUSED
		OPT_STAT(dce_frees)++;
		opline->opcode = ZEND_FREE;
		opline->op1.var = (uintptr_t) ZEND_CALL_VAR_NUM(NULL, ssa->vars[free_var].var);
		opline->op1_type = free_var_type;

		ssa_op->op1_use = free_var;
		ssa_op->op1_use_chain = ssa->vars[free_var].use_chain;
		ssa->vars[free_var].use_chain = ssa_op - ssa->ops;
	}
}

// TODO Move this somewhere else
static void simplify_jumps(zend_ssa *ssa, zend_op_array *op_array) {
	int i;
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		zend_ssa_op *ssa_op = &ssa->ops[i];
		zval *op1;

		/* Convert jump-and-set into jump if result is not used  */
		switch (opline->opcode) {
			case ZEND_JMPZ_EX:
				if (!var_used(&ssa->vars[ssa_op->result_def])) {
					opline->opcode = ZEND_JMPZ;
					remove_result_def(ssa, ssa_op);
				}
				break;
			case ZEND_JMPNZ_EX:
			case ZEND_JMP_SET:
				if (!var_used(&ssa->vars[ssa_op->result_def])) {
					opline->opcode = ZEND_JMPNZ;
					remove_result_def(ssa, ssa_op);
				}
				break;
			case ZEND_COALESCE:
				// TODO
				break;
		}

		if (opline->op1_type != IS_CONST) {
			continue;
		}

		/* Convert constant conditional jump to unconditional jump */
		op1 = &ZEND_OP1_LITERAL(opline);
		switch (opline->opcode) {
			case ZEND_JMPZ:
				if (!zend_is_true(op1)) {
					literal_dtor(op1);
					opline->op1_type = IS_UNUSED;
					opline->op1.num = opline->op2.num;
					opline->opcode = ZEND_JMP;
				} else {
					MAKE_NOP(opline);
				}
				break;
			case ZEND_JMPNZ:
				if (zend_is_true(op1)) {
					literal_dtor(op1);
					opline->op1_type = IS_UNUSED;
					opline->op1.num = opline->op2.num;
					opline->opcode = ZEND_JMP;
				} else {
					MAKE_NOP(opline);
				}
				break;
		}
	}
}

#if 0
static void simplify_jump_and_set(context *ctx) {
	switch (opline->opcode) {
		case ZEND_JMPZ_EX:
			/* For jump-and-set only the set part is dead */
			opline->opcode = ZEND_JMPZ;
			remove_result_def(ssa, ssa_op);

			/* Replace constant branch with JMP, so NOP pass may remove it */
			if (opline->op1_type == IS_CONST && !zend_is_true(&ZEND_OP1_LITERAL(opline))) {
				literal_dtor(&ZEND_OP1_LITERAL(opline));
				opline->opcode = ZEND_JMP;
				opline->op1_type = IS_UNUSED;
				opline->op1.num = opline->op2.num;
			}
			return;
		case ZEND_JMPNZ_EX:
		case ZEND_JMP_SET:
			opline->opcode = ZEND_JMPNZ;
			remove_result_def(ssa, ssa_op);

			if (opline->op1_type == IS_CONST && zend_is_true(&ZEND_OP1_LITERAL(opline))) {
				literal_dtor(&ZEND_OP1_LITERAL(opline));
				opline->opcode = ZEND_JMP;
				opline->op1_type = IS_UNUSED;
				opline->op1.num = opline->op2.num;
			}
			return;
		case ZEND_COALESCE:
			// TODO
			return;
	}
}
#endif

static inline int get_common_phi_source(zend_ssa *ssa, zend_ssa_phi *phi) {
	int common_source = -1;
	int source;
	FOREACH_PHI_SOURCE(phi, source) {
		if (common_source == -1) {
			common_source = source;
		} else if (common_source != source) {
			return -1;
		}
	} FOREACH_PHI_SOURCE_END();
	return common_source;
}

static void try_remove_trivial_phi(context *ctx, zend_ssa_phi *phi) {
	zend_ssa *ssa = ctx->ssa;
	if (phi->pi < 0) {
		int common_source = get_common_phi_source(ssa, phi);
		if (common_source >= 0) {
			rename_var_uses(ssa, phi->ssa_var, common_source);
			remove_phi(ssa, phi);
			OPT_STAT(dce_dead_phis)++;
		}
	}
}

void ssa_optimize_dce(ssa_opt_ctx *ssa_ctx) {
	zend_op_array *op_array = ssa_ctx->op_array;
	zend_ssa *ssa = ssa_ctx->ssa;
	int i;
	zend_ssa_phi *phi;
	/* DCE of CV operations may affect vararg functions. For now simply treat all instructions
	 * as live if varargs in use and only collect dead phis. */
	zend_bool has_varargs = (ZEND_FUNC_INFO(op_array)->flags & ZEND_FUNC_VARARG) != 0;

	context ctx;
	ctx.ssa = ssa;
	ctx.op_array = op_array;

	/* We have no dedicated phi vector, so we use the whole ssa var vector instead */
	ctx.instr_worklist_len = zend_bitset_len(op_array->last);
	ctx.instr_worklist = alloca(sizeof(zend_ulong) * ctx.instr_worklist_len);
	memset(ctx.instr_worklist, 0, sizeof(zend_ulong) * ctx.instr_worklist_len);
	ctx.phi_worklist_len = zend_bitset_len(ssa->vars_count);
	ctx.phi_worklist = alloca(sizeof(zend_ulong) * ctx.phi_worklist_len);
	memset(ctx.phi_worklist, 0, sizeof(zend_ulong) * ctx.phi_worklist_len);

	/* Optimistically assume all instructions and phis to be dead */
	ctx.instr_dead = alloca(sizeof(zend_ulong) * ctx.instr_worklist_len);
	memset(ctx.instr_dead, 0xff, sizeof(zend_ulong) * ctx.instr_worklist_len);
	ctx.phi_dead = alloca(sizeof(zend_ulong) * ctx.phi_worklist_len);
	memset(ctx.phi_dead, 0xff, sizeof(zend_ulong) * ctx.phi_worklist_len);

	/* Mark instruction with side effects as live */
	for (i = 0; i < op_array->last; ++i) {
		if (may_have_side_effects(op_array, ssa, &op_array->opcodes[i], &ssa->ops[i])
				|| has_varargs) {
			zend_bitset_excl(ctx.instr_dead, i);
			add_operands_to_worklists(&ctx, &op_array->opcodes[i], &ssa->ops[i]);
		}
	}

	/* Propagate liveness backwards to all definitions of used vars */
	while (!zend_bitset_empty(ctx.instr_worklist, ctx.instr_worklist_len)
			|| !zend_bitset_empty(ctx.phi_worklist, ctx.phi_worklist_len)) {
		while ((i = zend_bitset_pop_first(ctx.instr_worklist, ctx.instr_worklist_len)) >= 0) {
			zend_bitset_excl(ctx.instr_dead, i);
			add_operands_to_worklists(&ctx, &op_array->opcodes[i], &ssa->ops[i]);
		}
		while ((i = zend_bitset_pop_first(ctx.phi_worklist, ctx.phi_worklist_len)) >= 0) {
			zend_bitset_excl(ctx.phi_dead, i);
			add_phi_sources_to_worklists(&ctx, ssa->vars[i].definition_phi);
		}
	}

	/* Eliminate dead instructions */
	for (i = 0; i < op_array->last; ++i) {
		if (zend_bitset_in(ctx.instr_dead, i)) {
			dce_instr(&ctx, &op_array->opcodes[i], &ssa->ops[i]);
		}
	}

	/* Improper uses don't count as "uses" for the purpose of instruction elimination,
	 * but we have to retain phis defining them. Push those phis to the worklist. */
	for (i = 0; i < op_array->last; i++) {
		if (has_improper_op1_use(&op_array->opcodes[i])) {
			ZEND_ASSERT(ssa->ops[i].op1_use >= 0);
			add_to_phi_worklist_only(&ctx, ssa->ops[i].op1_use);
		}
	}

	/* Propagate this information backwards, marking any phi with an improperly used
	 * target as non-dead. */
	while ((i = zend_bitset_pop_first(ctx.phi_worklist, ctx.phi_worklist_len)) >= 0) {
		zend_ssa_phi *phi = ssa->vars[i].definition_phi;
		int source;
		zend_bitset_excl(ctx.phi_dead, i);
		FOREACH_PHI_SOURCE(phi, source) {
			add_to_phi_worklist_only(&ctx, source);
		} FOREACH_PHI_SOURCE_END();
	}

	/* Now collect the actually dead phis */
	FOREACH_PHI(phi) {
		if (zend_bitset_in(ctx.phi_dead, phi->ssa_var)) {
			OPT_STAT(dce_dead_phis)++;
			remove_uses_of_var(ssa, phi->ssa_var);
			remove_phi(ssa, phi);
		}
	} FOREACH_PHI_END();

	/* Remove trivial phis (phis with identical source operands) */
	FOREACH_PHI(phi) {
		try_remove_trivial_phi(&ctx, phi);
	} FOREACH_PHI_END();

	simplify_jumps(ssa, op_array);

	for (i = 0; i < op_array->last; ++i) {
		zend_ssa_op *ssa_op = &ssa->ops[i];
		zend_op *opline = &op_array->opcodes[i];
		if (!may_have_side_effects(op_array, ssa, opline, ssa_op)) {
			continue;
		}
		if (ssa_op->op1_use < 0 && ssa_op->op2_use < 0 && ssa_op->result_use < 0) {
			continue;
		}
		if ((ssa_op->op1_def >= 0 && var_used(&ssa->vars[ssa_op->op1_def])) ||
			(ssa_op->op2_def >= 0 && var_used(&ssa->vars[ssa_op->op2_def])) ||
			(ssa_op->result_def >= 0 && var_used(&ssa->vars[ssa_op->result_def]))
		) {
			continue;
		}
		if (opline->opcode == ZEND_OP_DATA) {
			opline--;
		}
		fprintf(stderr, "ROOT %s\n", zend_get_opcode_name(opline->opcode));
	}
}
