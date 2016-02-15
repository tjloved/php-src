#include "ZendAccelerator.h"
#include "Optimizer/ssa/helpers.h"

static void propagate_phi_type_widening(zend_ssa *ssa, int var) {
	zend_ssa_phi *phi;
	FOREACH_PHI_USE(&ssa->vars[var], phi) {
		if (ssa->var_info[phi->ssa_var].type & ~ssa->var_info[var].type) {
			ssa->var_info[phi->ssa_var].type |= ssa->var_info[var].type;
			propagate_phi_type_widening(ssa, phi->ssa_var);
		}
	} FOREACH_PHI_USE_END();
}

void rename_var_uses(zend_ssa *ssa, int old, int new) {
	zend_ssa_var *old_var = &ssa->vars[old];
	zend_ssa_var *new_var = &ssa->vars[new];
	int use;
	zend_ssa_phi *phi;

	ZEND_ASSERT(old >= 0 && new >= 0);
	ZEND_ASSERT(old != new);

	/* Only a no_val is both variables are */
	new_var->no_val &= old_var->no_val;

	/* Update ssa_op use chains */
	FOREACH_USE(old_var, use) {
		zend_ssa_op *ssa_op = &ssa->ops[use];

		/* If the op already uses the new var, don't add the op to the use
		 * list again. Instead move the use_chain to the correct operand. */
		zend_bool add_to_use_chain = 1;
		if (ssa_op->result_use == new) {
			add_to_use_chain = 0;
		} else if (ssa_op->op1_use == new) {
			if (ssa_op->result_use == old) {
				ssa_op->res_use_chain = ssa_op->op1_use_chain;
				ssa_op->op1_use_chain = -1;
			}
			add_to_use_chain = 0;
		} else if (ssa_op->op2_use == new) {
			if (ssa_op->result_use == old) {
				ssa_op->res_use_chain = ssa_op->op2_use_chain;
				ssa_op->op2_use_chain = -1;
			} else if (ssa_op->op1_use == old) {
				ssa_op->op1_use_chain = ssa_op->op2_use_chain;
				ssa_op->op2_use_chain = -1;
			}
			add_to_use_chain = 0;
		}

		/* Perform the actual renaming */
		if (ssa_op->result_use == old) {
			ssa_op->result_use = new;
		}
		if (ssa_op->op1_use == old) {
			ssa_op->op1_use = new;
		}
		if (ssa_op->op2_use == old) {
			ssa_op->op2_use = new;
		}

		/* Add op to use chain of new var (if it isn't already). We use the
		 * first use chain of (result, op1, op2) that has the new variable. */
		if (add_to_use_chain) {
			if (ssa_op->result_use == new) {
				ssa_op->res_use_chain = new_var->use_chain;
				new_var->use_chain = use;
			} else if (ssa_op->op1_use == new) {
				ssa_op->op1_use_chain = new_var->use_chain;
				new_var->use_chain = use;
			} else {
				ZEND_ASSERT(ssa_op->op2_use == new);
				ssa_op->op2_use_chain = new_var->use_chain;
				new_var->use_chain = use;
			}
		}
	} FOREACH_USE_END();
	old_var->use_chain = -1;

	/* Update phi use chains */
	FOREACH_PHI_USE(old_var, phi) {
		int j;
		zend_bool after_first_new_source = 0;

		/* If the phi already uses the new var, find its use chain, as we may
		 * need to move it to a different source operand. */
		zend_ssa_phi **existing_use_chain_ptr = NULL;
		for (j = 0; j < ssa->cfg.blocks[phi->block].predecessors_count; j++) {
			if (phi->sources[j] == new) {
				existing_use_chain_ptr = &phi->use_chains[j];
				break;
			}
		}

		for (j = 0; j < ssa->cfg.blocks[phi->block].predecessors_count; j++) {
			if (phi->sources[j] == new) {
				after_first_new_source = 1;
			} else if (phi->sources[j] == old) {
				phi->sources[j] = new;

				/* Either move existing use chain to this source, or add the phi
				 * to the phi use chain of the new variables. Do this only once. */
				if (!after_first_new_source) {
					if (existing_use_chain_ptr) {
						phi->use_chains[j] = *existing_use_chain_ptr;
						*existing_use_chain_ptr = NULL;
					} else {
						phi->use_chains[j] = new_var->phi_use_chain;
						new_var->phi_use_chain = phi;
					}
					after_first_new_source = 1;
				}
			}
		}

		/* Make sure phi result types are not incorrectly narrow after renaming.
		 * This should not normally happen, but can occur if we DCE an assignment
		 * or unset and there is an improper phi-indirected use lateron. */
		// TODO Alternatively we could rerun type-inference after DCE
		if (ssa->var_info[phi->ssa_var].type & ~ssa->var_info[new].type) {
			ssa->var_info[phi->ssa_var].type |= ssa->var_info[new].type;
			propagate_phi_type_widening(ssa, phi->ssa_var);
		}
	} FOREACH_PHI_USE_END();
	old_var->phi_use_chain = NULL;
}

static zend_always_inline void remove_use(zend_ssa *ssa, int use, int var, int op) {
	int *cur = &ssa->vars[var].use_chain;
	while (*cur >= 0) {
		zend_ssa_op *ssa_op = &ssa->ops[*cur];

		int *next;
		if (ssa_op->result_use == var) {
			next = &ssa_op->res_use_chain;
		} else if (ssa_op->op1_use == var) {
			next = &ssa_op->op1_use_chain;
		} else {
			ZEND_ASSERT(ssa_op->op2_use == var);
			next = &ssa_op->op2_use_chain;
		}

		if (*cur == use) {
			if (op == 0) {
				ssa_op->result_use = -1;
			} else if (op == 1) {
				ssa_op->op1_use = -1;
				if (ssa_op->op2_use == var) {
					/* Op2 uses same var -- switch to op2 use-chain */
					ssa_op->op2_use_chain = ssa_op->op1_use_chain;
					ssa_op->op1_use_chain = -1;
					return;
				}
			} else {
				ZEND_ASSERT(op == 2);
				ssa_op->op2_use = -1;
				if (ssa_op->op1_use == var) {
					/* Op1 uses same var -- just keep the use-chain as is */
					return;
				}
			}

			*cur = *next;
			return;
		}
		cur = next;
	}
}

void remove_result_use(zend_ssa *ssa, zend_ssa_op *ssa_op) {
	remove_use(ssa, ssa_op - ssa->ops, ssa_op->result_use, 0);
}
void remove_op1_use(zend_ssa *ssa, zend_ssa_op *ssa_op) {
	remove_use(ssa, ssa_op - ssa->ops, ssa_op->op1_use, 1);
}
void remove_op2_use(zend_ssa *ssa, zend_ssa_op *ssa_op) {
	remove_use(ssa, ssa_op - ssa->ops, ssa_op->op2_use, 2);
}

static inline zend_ssa_phi **next_use_phi_ptr(zend_ssa *ssa, int var, zend_ssa_phi *p) {
	if (p->pi >= 0) {
		return &p->use_chains[0];
	} else {
		int j;
		for (j = 0; j < ssa->cfg.blocks[p->block].predecessors_count; j++) {
			if (p->sources[j] == var) {
				return &p->use_chains[j];
			}
		}
	}
	ZEND_ASSERT(0);
	return NULL;
}

static void remove_uses_of_phi_sources(zend_ssa *ssa, zend_ssa_phi *phi) {
	int source;
	FOREACH_PHI_SOURCE(phi, source) {
		zend_ssa_phi **cur = &ssa->vars[source].phi_use_chain;
		while (*cur && *cur != phi) {
			cur = next_use_phi_ptr(ssa, source, *cur);
		}
		/* *cur can be NULL here if the Phi uses the same source multiple times
		 * and it was already removed in a previous iteration. */
		if (*cur) {
			*cur = zend_ssa_next_use_phi(ssa, source, *cur);
		}
	} FOREACH_PHI_SOURCE_END();
}

static void remove_phi_from_block(zend_ssa *ssa, zend_ssa_phi *phi) {
	zend_ssa_block *block = &ssa->blocks[phi->block];
	zend_ssa_phi **cur = &block->phis;
	while (*cur != phi) {
		ZEND_ASSERT(*cur != NULL);
		cur = &(*cur)->next;
	}
	*cur = (*cur)->next;
}

void remove_phi(zend_ssa *ssa, zend_ssa_phi *phi) {
	ZEND_ASSERT(phi->ssa_var >= 0);
	ZEND_ASSERT(!var_used(&ssa->vars[phi->ssa_var]));
	remove_uses_of_phi_sources(ssa, phi);
	remove_phi_from_block(ssa, phi);
	ssa->vars[phi->ssa_var].definition_phi = NULL;
	phi->ssa_var = -1;
}

void remove_uses_of_var(zend_ssa *ssa, int var_num) {
	zend_ssa_var *var = &ssa->vars[var_num];
	zend_ssa_phi *phi;
	int use;
	FOREACH_PHI_USE(var, phi) {
		int i, end = NUM_PHI_SOURCES(phi);
		for (i = 0; i < end; i++) {
			if (phi->sources[i] == var_num) {
				phi->sources[i] = -1;
				phi->use_chains[i] = NULL;
			}
		}
	} FOREACH_PHI_USE_END();
	var->phi_use_chain = NULL;
	FOREACH_USE(var, use) {
		zend_ssa_op *ssa_op = &ssa->ops[use];
		if (ssa_op->op1_use == var_num) {
			ssa_op->op1_use = -1;
			ssa_op->op1_use_chain = -1;
		}
		if (ssa_op->op2_use == var_num) {
			ssa_op->op2_use = -1;
			ssa_op->op2_use_chain = -1;
		}
		if (ssa_op->result_use == var_num) {
			ssa_op->result_use = -1;
			ssa_op->res_use_chain = -1;
		}
	} FOREACH_USE_END();
	var->use_chain = -1;
}

void remove_block(zend_ssa *ssa, int i, uint32_t *num_instr, uint32_t *num_phi) {
	zend_op_array *op_array = ssa->op_array;
	zend_basic_block *block = &ssa->cfg.blocks[i];
	zend_ssa_block *ssa_block = &ssa->blocks[i];
	zend_ssa_phi *phi;
	int j;

	block->flags &= ~ZEND_BB_REACHABLE;
	for (phi = ssa_block->phis; phi; phi = phi->next) {
		remove_uses_of_var(ssa, phi->ssa_var);
		remove_phi(ssa, phi);
		(*num_phi)++;
	}
	for (j = block->start; j <= block->end; j++) {
		if (op_array->opcodes[j].opcode == ZEND_NOP) {
			continue;
		}

		remove_defs_of_instr(ssa, &ssa->ops[j]);
		remove_instr(ssa, &op_array->opcodes[j], &ssa->ops[j]);
		(*num_instr)++;
	}
}

