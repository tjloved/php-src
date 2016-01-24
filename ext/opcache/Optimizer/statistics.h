#ifndef _OPTIMIZER_STATISTICS_H
#define _OPTIMIZER_STATISTICS_H

typedef struct _zend_optimizer_statistics {
	uint32_t instrs;
	uint32_t cfg_blocks;
	uint32_t ssa_vars;
	uint32_t ssa_may_be_ref;
	uint32_t ssa_may_be_any;
	uint32_t ssa_may_be_refcounted;
	uint32_t cv_ssa_vars;
	uint32_t cv_ssa_may_be_ref;
	uint32_t cv_ssa_may_be_any;
	uint32_t cv_ssa_may_be_refcounted;
	uint32_t cv_ssa_may_be_undef;
	uint32_t cv_ssa_must_be_undef;
	uint32_t scp_const_vars;
	uint32_t scp_dead_blocks;
	uint32_t scp_dead_instrs;
	uint32_t scp_semi_dead_instrs;
	uint32_t dce_dead_instr;
	uint32_t copy_contracted_assign;
	uint32_t copy_qm_assign;
	uint32_t type_spec_arithm;
	uint32_t type_spec_must_be_array;
	uint32_t type_spec_not_known_to_be_array;
	uint32_t type_spec_must_be_int_key;
	uint32_t type_spec_must_be_string_key;
	uint32_t type_spec_must_be_matching_int_key;
	uint32_t type_spec_must_be_append_int_key;
	uint32_t type_spec_must_be_notref_array_values;
	uint32_t scp_dead_blocks_instrs;
	uint32_t scp_dead_blocks_phis;
	uint32_t dce_dead_phis;
	uint32_t scp_dead_phis;
	uint32_t inlining_funcs;
	uint32_t inlining_instrs;
	uint32_t phis;
	uint32_t trivial_phis;
	uint32_t copy_propagated;
} zend_optimizer_statistics;

extern zend_optimizer_statistics optimizer_statistics;

#define OPT_STAT(name) (optimizer_statistics.name)

void zend_optimizer_statistics_startup(void);
void zend_optimizer_statistics_shutdown(void);

#endif

