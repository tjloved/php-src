#include "php.h"
#include "ZendAccelerator.h"
#include "Optimizer/statistics.h"

zend_optimizer_statistics optimizer_statistics;

#define PRINT_STAT(name) \
	fprintf(stderr, "%-27s: %6" PRIu32 "\n", #name, OPT_STAT(name))
#define PRINT_NORM_STAT(name, norm) \
	fprintf(stderr, "%-27s: %6" PRIu32 "   (%5.2f%% " #norm ")\n", \
		#name, OPT_STAT(name), (float) OPT_STAT(name) / OPT_STAT(norm) * 100);

void zend_optimizer_statistics_startup() {}
void zend_optimizer_statistics_shutdown() {
	if (!OPT_STAT_ENABLED()) {
		return;
	}

	PRINT_STAT(instrs);
	PRINT_STAT(cfg_blocks);
	PRINT_STAT(phis);
	PRINT_STAT(pis);

	fprintf(stderr, "%-27s: %f\n", "type_quality",
		OPT_STAT(type_quality) / (OPT_STAT(ssa_vars) - OPT_STAT(ssa_may_be_nothing)));

	PRINT_STAT(ssa_vars);
	PRINT_NORM_STAT(ssa_may_be_ref, ssa_vars);
	PRINT_NORM_STAT(ssa_may_be_any, ssa_vars);
	PRINT_NORM_STAT(ssa_may_be_refcounted, ssa_vars);
	PRINT_NORM_STAT(ssa_may_be_nothing, ssa_vars);

	PRINT_STAT(cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_may_be_ref, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_may_be_any, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_may_be_refcounted, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_may_be_undef, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_must_be_undef, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_one_type, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_two_types, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_3_types, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_4_types, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_56_types, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_7_types, cv_ssa_vars);
	PRINT_NORM_STAT(cv_ssa_8_types, cv_ssa_vars);

	PRINT_STAT(inlining_funcs);
	PRINT_NORM_STAT(inlining_instrs, instrs);
	PRINT_STAT(inlining_arg_assigns);

	PRINT_STAT(cloned_funcs);

	PRINT_NORM_STAT(trivial_phis, phis);

	PRINT_NORM_STAT(ti_dead_blocks, cfg_blocks);
	PRINT_NORM_STAT(ti_dead_blocks_instrs, instrs);
	PRINT_NORM_STAT(ti_dead_blocks_phis, phis);

	PRINT_NORM_STAT(scp_const_vars, ssa_vars);
	PRINT_STAT(scp_const_operands);
	PRINT_NORM_STAT(scp_dead_instrs, instrs);
	PRINT_NORM_STAT(scp_semi_dead_instrs, instrs);
	PRINT_NORM_STAT(scp_dead_phis, phis);
	PRINT_STAT(scp_dead_calls);
	PRINT_NORM_STAT(scp_dead_blocks, cfg_blocks);
	PRINT_NORM_STAT(scp_dead_blocks_instrs, instrs);
	PRINT_NORM_STAT(scp_dead_blocks_phis, phis);

	PRINT_NORM_STAT(dce_dead_instr, instrs);
	PRINT_NORM_STAT(dce_dead_phis, phis);
	PRINT_STAT(dce_frees);
	PRINT_NORM_STAT(cfg_merged_blocks, cfg_blocks);

	PRINT_NORM_STAT(assign_contracted_assign, instrs);
	PRINT_NORM_STAT(assign_qm_assign, instrs);
	PRINT_NORM_STAT(copy_propagated_cv, instrs);
	PRINT_NORM_STAT(copy_propagated_tmp, instrs);
	PRINT_NORM_STAT(type_spec_arithm, instrs);
	PRINT_NORM_STAT(type_spec_elided, instrs);
	PRINT_NORM_STAT(type_spec_send, instrs);
	PRINT_STAT(ts_must_be_array);
	PRINT_STAT(ts_not_must_be_array);
	PRINT_STAT(ts_must_be_int_key);
	PRINT_STAT(ts_must_be_string_key);
	PRINT_STAT(ts_must_be_matching_int_key);
	PRINT_STAT(ts_must_be_append_int_key);
	PRINT_STAT(ts_must_be_notref_values);

	PRINT_STAT(vars_orig_cvs);
	PRINT_NORM_STAT(vars_dead_cvs, vars_orig_cvs);

	PRINT_STAT(simplified_sends);

	PRINT_STAT(tmp);
	PRINT_STAT(tmp2);
	PRINT_STAT(tmp3);
}
