#include "php.h"
#include "ZendAccelerator.h"
#include "Optimizer/statistics.h"

zend_optimizer_statistics optimizer_statistics;

#define PRINT_STAT(name) \
	fprintf(stderr, "%-25s: %6" PRIu32 "\n", #name, OPT_STAT(name))
#define PRINT_NORM_STAT(name, norm) \
	fprintf(stderr, "%-25s: %6" PRIu32 "   (%5.2f%% " #norm ")\n", \
		#name, OPT_STAT(name), (float) OPT_STAT(name) / OPT_STAT(norm) * 100);

void zend_optimizer_statistics_startup() {}
void zend_optimizer_statistics_shutdown() {
	if (!ZCG(accel_directives).opt_statistics) {
		return;
	}

	PRINT_STAT(instrs);
	PRINT_STAT(cfg_blocks);

	PRINT_STAT(ssa_vars);
	PRINT_NORM_STAT(ssa_may_be_ref, ssa_vars);
	PRINT_NORM_STAT(ssa_may_be_any, ssa_vars);
	PRINT_NORM_STAT(ssa_may_be_refcounted, ssa_vars);

	PRINT_STAT(cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_may_be_ref, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_may_be_any, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_may_be_refcounted, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_may_be_undef, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_must_be_undef, cv_ssa_vars);

	PRINT_NORM_STAT(scp_const_vars, ssa_vars);
	PRINT_NORM_STAT(scp_dead_blocks, cfg_blocks);
	PRINT_NORM_STAT(scp_dead_instrs, instrs);
	PRINT_NORM_STAT(scp_semi_dead_instrs, instrs);
	PRINT_NORM_STAT(dce_dead_instr, instrs);
	PRINT_STAT(copy_contracted_assign);
	PRINT_STAT(copy_qm_assign);
	PRINT_STAT(type_spec_arithm);
	PRINT_STAT(type_spec_must_be_array);
	PRINT_STAT(type_spec_not_known_to_be_array);
	PRINT_STAT(type_spec_must_be_int_key);
	PRINT_STAT(type_spec_must_be_string_key);
	PRINT_STAT(type_spec_must_be_matching_int_key);
	PRINT_STAT(type_spec_must_be_append_int_key);
	PRINT_STAT(type_spec_must_be_notref_array_values);
}
