#ifndef SSA_HELPERS_H
#define SSA_HELPERS_H

#include "Optimizer/zend_ssa.h"
#include "Optimizer/zend_inference.h"
#include "Optimizer/zend_call_graph.h"

#define CAN_BE(t, types) ((t) & (types))
#define MUST_BE(t, types) (!((t & MAY_BE_ANY) & ~(types)))

#define MAY_BE_REFCOUNTED (MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_OBJECT|MAY_BE_RESOURCE)

#define NUM_PHI_SOURCES(phi) \
	((phi)->pi >= 0 ? 1 : (ssa->cfg.blocks[(phi)->block].predecessors_count))

/* FOREACH_USE and FOREACH_PHI_USE explicitly support "continue"
 * and changing the use chain of the current element */
#define FOREACH_USE(var, use) do { \
	int _var_num = (var) - ssa->vars, next; \
	for (use = (var)->use_chain; use >= 0; use = next) { \
		next = zend_ssa_next_use(ssa->ops, _var_num, use);
#define FOREACH_USE_END() \
	} \
} while (0)

#define FOREACH_PHI_USE(var, phi) do { \
	int _var_num = (var) - ssa->vars; \
	zend_ssa_phi *next_phi; \
	for (phi = (var)->phi_use_chain; phi; phi = next_phi) { \
		next_phi = zend_ssa_next_use_phi(ssa, _var_num, phi);
#define FOREACH_PHI_USE_END() \
	} \
} while (0)

#define FOREACH_PHI_SOURCE(phi, source) do { \
	zend_ssa_phi *_phi = (phi); \
	int _i, _end = NUM_PHI_SOURCES(phi); \
	for (_i = 0; _i < _end; _i++) { \
		if (_phi->sources[_i] < 0) continue; \
		source = _phi->sources[_i];
#define FOREACH_PHI_SOURCE_END() \
	} \
} while (0)

#define FOREACH_PHI(phi) do { \
	int _i; \
	for (_i = 0; _i < ssa->cfg.blocks_count; _i++) { \
		phi = ssa->blocks[_i].phis; \
		for (; phi; phi = phi->next) { \
			//if (phi->ssa_var < 0) continue;
#define FOREACH_PHI_END() \
		} \
	} \
} while (0)

void rename_var_uses(zend_ssa *ssa, int old, int new);
void remove_result_use(zend_ssa *ssa, zend_ssa_op *ssa_op);
void remove_op1_use(zend_ssa *ssa, zend_ssa_op *ssa_op);
void remove_op2_use(zend_ssa *ssa, zend_ssa_op *ssa_op);
void remove_phi(zend_ssa *ssa, zend_ssa_phi *phi);
void remove_uses_of_var(zend_ssa *ssa, int var_num);

static inline void set_op1_use(zend_ssa *ssa, zend_ssa_op *ssa_op, int var_num) {
	zend_ssa_var *var = &ssa->vars[var_num];
	if (ssa_op->op1_use >= 0) {
		remove_op1_use(ssa, ssa_op);
	}
	ssa_op->op1_use = var_num;
	if (ssa_op->result_use == var_num) {
		return;
	}
	if (ssa_op->op2_use == var_num) {
		ssa_op->op1_use_chain = ssa_op->op2_use_chain;
		ssa_op->op2_use_chain = -1;
		return;
	}
	ssa_op->op1_use_chain = var->use_chain;
	var->use_chain = ssa_op - ssa->ops;
}

static inline void _remove_def(zend_ssa_var *var) {
	ZEND_ASSERT(var->definition >= 0);
	ZEND_ASSERT(var->use_chain < 0);
	ZEND_ASSERT(!var->phi_use_chain);
	var->definition = -1;
}
static inline void remove_result_def(zend_ssa *ssa, zend_ssa_op *ssa_op) {
	zend_ssa_var *var = &ssa->vars[ssa_op->result_def];
	_remove_def(var);
	ssa_op->result_def = -1;
}
static inline void remove_op1_def(zend_ssa *ssa, zend_ssa_op *ssa_op) {
	zend_ssa_var *var = &ssa->vars[ssa_op->op1_def];
	_remove_def(var);
	ssa_op->op1_def = -1;
}
static inline void remove_op2_def(zend_ssa *ssa, zend_ssa_op *ssa_op) {
	zend_ssa_var *var = &ssa->vars[ssa_op->op2_def];
	_remove_def(var);
	ssa_op->op2_def = -1;
}

static inline zend_bool var_used(zend_ssa_var *var) {
	// TODO Do we care about sym_use_chain at this point?
	return var->use_chain >= 0 || var->phi_use_chain != NULL;
}

static inline void remove_defs_of_instr(zend_ssa *ssa, zend_ssa_op *ssa_op) {
	if (ssa_op->op1_def >= 0) {
		remove_uses_of_var(ssa, ssa_op->op1_def);
		remove_op1_def(ssa, ssa_op);
	}
	if (ssa_op->op2_def >= 0) {
		remove_uses_of_var(ssa, ssa_op->op2_def);
		remove_op2_def(ssa, ssa_op);
	}
	if (ssa_op->result_def >= 0) {
		remove_uses_of_var(ssa, ssa_op->result_def);
		remove_result_def(ssa, ssa_op);
	}
}

static inline void rename_defs_of_instr(zend_ssa *ssa, zend_ssa_op *ssa_op) {
	/* Rename def to use if possible. Mark variable as not defined otherwise. */
	if (ssa_op->op1_def >= 0) {
		if (ssa_op->op1_use >= 0) {
			rename_var_uses(ssa, ssa_op->op1_def, ssa_op->op1_use);
		}
		ssa->vars[ssa_op->op1_def].definition = -1;
		ssa_op->op1_def = -1;
	}
	if (ssa_op->op2_def >= 0) {
		if (ssa_op->op2_use >= 0) {
			rename_var_uses(ssa, ssa_op->op2_def, ssa_op->op2_use);
		}
		ssa->vars[ssa_op->op2_def].definition = -1;
		ssa_op->op2_def = -1;
	}
	if (ssa_op->result_def >= 0) {
		if (ssa_op->result_use >= 0) {
			rename_var_uses(ssa, ssa_op->result_def, ssa_op->result_use);
		}
		ssa->vars[ssa_op->result_def].definition = -1;
		ssa_op->result_def = -1;
	}
}

static inline void remove_instr(zend_ssa *ssa, zend_op *opline, zend_ssa_op *ssa_op) {
	if (ssa_op->result_use >= 0) {
		remove_result_use(ssa, ssa_op);
	}
	if (ssa_op->op1_use >= 0) {
		remove_op1_use(ssa, ssa_op);
	}
	if (ssa_op->op2_use >= 0) {
		remove_op2_use(ssa, ssa_op);
	}

	/* We let the caller make sure that all defs are gone */
	ZEND_ASSERT(ssa_op->result_def == -1);
	ZEND_ASSERT(ssa_op->op1_def == -1);
	ZEND_ASSERT(ssa_op->op2_def == -1);

	MAKE_NOP(opline);
}

static inline int zend_bitset_pop_first(zend_bitset set, uint32_t len) {
	int i = zend_bitset_first(set, len);
	if (i >= 0) {
		zend_bitset_excl(set, i);
	}
	return i;
}

#endif

