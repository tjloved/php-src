#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"

static inline zend_bool is_in_use_chain(zend_ssa *ssa, int var, int check) {
	int use;
	FOREACH_USE(&ssa->vars[var], use) {
		if (use == check) {
			return 1;
		}
	} FOREACH_USE_END();
	return 0;
}

static inline zend_bool is_in_phi_use_chain(zend_ssa *ssa, int var, zend_ssa_phi *check) {
	zend_ssa_phi *phi;
	FOREACH_PHI_USE(&ssa->vars[var], phi) {
		if (phi == check) {
			return 1;
		}
	} FOREACH_PHI_USE_END();
	return 0;
}

static inline zend_bool is_used_by_op(zend_ssa *ssa, int op, int check) {
	zend_ssa_op *ssa_op = &ssa->ops[op];
	return (ssa_op->op1_use == check)
		|| (ssa_op->op2_use == check)
		|| (ssa_op->result_use == check);
}

static inline zend_bool is_defined_by_op(zend_ssa *ssa, int op, int check) {
	zend_ssa_op *ssa_op = &ssa->ops[op];
	return (ssa_op->op1_def == check)
		|| (ssa_op->op2_def == check)
		|| (ssa_op->result_def == check);
}

static inline zend_bool is_in_phi_sources(zend_ssa *ssa, zend_ssa_phi *phi, int check) {
	int source;
	FOREACH_PHI_SOURCE(phi, source) {
		if (source == check) {
			return 1;
		}
	} FOREACH_PHI_SOURCE_END();
	return 0;
}

#define FAIL(...) do { \
	if (status == SUCCESS) { \
		fprintf(stderr, "\nIn function %s (%s):\n", \
			op_array->function_name ? ZSTR_VAL(op_array->function_name) : "{main}", extra); \
	} \
	fprintf(stderr, __VA_ARGS__); \
	status = FAILURE; \
} while (0)

#define VARFMT "%d (%s%s)"
#define VAR(i) \
	(i), (ssa->vars[i].var < op_array->last_var ? "CV $" : "TMP"), \
	(ssa->vars[i].var < op_array->last_var ? ZSTR_VAL(op_array->vars[ssa->vars[i].var]) : "")

int ssa_verify_integrity(zend_ssa *ssa, const char *extra) {
	zend_op_array *op_array = ssa->op_array;
	zend_ssa_phi *phi;
	int i, status = SUCCESS;
	for (i = 0; i < ssa->vars_count; i++) {
		zend_ssa_var *var = &ssa->vars[i];
		int use, c;

		if (var->definition < 0 && !var->definition_phi && i > op_array->last_var) {
			if (var->use_chain >= 0) {
				FAIL("var " VARFMT " without def has op uses\n", VAR(i));
			}
			if (var->phi_use_chain) {
				FAIL("var " VARFMT " without def has phi uses\n", VAR(i));
			}
		}
		if (var->definition >= 0 && var->definition_phi) {
			FAIL("var " VARFMT " has both def and def_phi\n", VAR(i));
		}
		if (var->definition >= 0) {
			if (!is_defined_by_op(ssa, var->definition, i)) {
				FAIL("var " VARFMT " not defined by op %d\n", VAR(i), var->definition);
			}
		}
		if (var->definition_phi) {
			if (var->definition_phi->ssa_var != i) {
				FAIL("var " VARFMT " not defined by given phi\n", VAR(i));
			}
		}

		c = 0;
		FOREACH_USE(var, use) {
			if (++c > 10000) {
				FAIL("cycle in uses of " VARFMT "\n", VAR(i));
				return status;
			}
			if (!is_used_by_op(ssa, use, i)) {
				fprintf(stderr, "var " VARFMT " not in uses of op %d\n", VAR(i), use);
			}
		} FOREACH_USE_END();

		c = 0;
		FOREACH_PHI_USE(var, phi) {
			if (++c > 10000) {
				FAIL("cycle in phi uses of " VARFMT "\n", VAR(i));
				return status;
			}
			if (!is_in_phi_sources(ssa, phi, i)) {
				FAIL("var " VARFMT " not in phi sources of %d\n", VAR(i), phi->ssa_var);
			}
		} FOREACH_PHI_USE_END();
	}
	for (i = 0; i < op_array->last; i++) {
		zend_ssa_op *ssa_op = &ssa->ops[i];
		if (ssa_op->op1_use >= 0) {
			if (!is_in_use_chain(ssa, ssa_op->op1_use, i)) {
				FAIL("op1 use of " VARFMT " in %d not in use chain\n", VAR(ssa_op->op1_use), i);
			}
		}
		if (ssa_op->op2_use >= 0) {
			if (!is_in_use_chain(ssa, ssa_op->op2_use, i)) {
				FAIL("op1 use of " VARFMT " in %d not in use chain\n", VAR(ssa_op->op2_use), i);
			}
		}
		if (ssa_op->result_use >= 0) {
			if (!is_in_use_chain(ssa, ssa_op->result_use, i)) {
				FAIL("result use of " VARFMT " in %d not in use chain\n",
					VAR(ssa_op->result_use), i);
			}
		}
		if (ssa_op->op1_def >= 0) {
			if (ssa->vars[ssa_op->op1_def].definition != i) {
				FAIL("op1 def of " VARFMT " in %d invalid\n", VAR(ssa_op->op1_def), i);
			}
		}
		if (ssa_op->op2_def >= 0) {
			if (ssa->vars[ssa_op->op2_def].definition != i) {
				FAIL("op2 def of " VARFMT " in %d invalid\n", VAR(ssa_op->op2_def), i);
			}
		}
		if (ssa_op->result_def >= 0) {
			if (ssa->vars[ssa_op->result_def].definition != i) {
				FAIL("result def of " VARFMT " in %d invalid\n", VAR(ssa_op->result_def), i);
			}
		}
	}
	FOREACH_PHI(phi) {
		int source;
		FOREACH_PHI_SOURCE(phi, source) {
			if (!is_in_phi_use_chain(ssa, source, phi)) {
				FAIL(VARFMT " not in phi use chain of %d\n", VAR(phi->ssa_var), source);
			}
		} FOREACH_PHI_SOURCE_END();
		if (ssa->vars[phi->ssa_var].definition_phi != phi) {
			FAIL(VARFMT " does not define this phi\n", VAR(phi->ssa_var));
		}
	} FOREACH_PHI_END();
	return status;
}
