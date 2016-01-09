#include "php.h"
#include "ZendAccelerator.h"
#include "Optimizer/statistics.h"

zend_optimizer_statistics optimizer_statistics;

#define PRINT_STAT(name) fprintf(stderr, #name ": %" PRIu32 "\n", OPT_STAT(name))

void zend_optimizer_statistics_startup() {}
void zend_optimizer_statistics_shutdown() {
	if (!ZCG(accel_directives).opt_statistics) {
		return;
	}

	PRINT_STAT(instrs);
	PRINT_STAT(cfg_blocks);
	PRINT_STAT(ssa_vars);
	PRINT_STAT(ssa_may_be_ref);
	PRINT_STAT(ssa_may_be_any);
	PRINT_STAT(ssa_may_be_refcounted);
	PRINT_STAT(cv_ssa_vars);
	PRINT_STAT(cv_ssa_may_be_ref);
	PRINT_STAT(cv_ssa_may_be_any);
	PRINT_STAT(cv_ssa_may_be_refcounted);
	PRINT_STAT(cv_ssa_may_be_undef);
	PRINT_STAT(scp_const_vars);
	PRINT_STAT(scp_dead_blocks);
	PRINT_STAT(scp_dead_instrs);
	PRINT_STAT(scp_semi_dead_instrs);
	PRINT_STAT(dce_dead_instr);
	PRINT_STAT(copy_contracted_assign);
	PRINT_STAT(copy_qm_assign);
}
