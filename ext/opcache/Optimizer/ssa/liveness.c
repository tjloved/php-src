#include "php.h"
#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa/helpers.h"
//#include "Optimizer/ssa/instructions.h"
//#include "Optimizer/statistics.h"

/* This implements "Fast Liveness Checking for SSA-Form Programs" by Boissinot et al. */

typedef struct _ssa_liveness {
	zend_ssa *ssa;
} ssa_liveness;

zend_bool is_live_in(ssa_liveness *liveness, int var, int block) {
	return -1;
}
zend_bool is_live_out(ssa_liveness *liveness, int var, int block) {
	return -1;
}
