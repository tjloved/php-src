#ifndef SSA_INSTRUCTIONS_H
#define SSA_INSTRUCTIONS_H

/* Whether the opline might throw an exception or a diagnostic, excluding
 * exceptions thrown by destruction of the opline operands. */
zend_bool may_throw(
	zend_op_array *op_array, zend_ssa *ssa, const zend_op *opline, const zend_ssa_op *ssa_op);

static inline zend_uchar instr_get_compound_assign_op(zend_op *opline) {
	switch (opline->opcode) {
		case ZEND_ASSIGN_ADD: return ZEND_ADD;
		case ZEND_ASSIGN_SUB: return ZEND_SUB;
		case ZEND_ASSIGN_MUL: return ZEND_MUL;
		case ZEND_ASSIGN_DIV: return ZEND_DIV;
		case ZEND_ASSIGN_MOD: return ZEND_MOD;
		case ZEND_ASSIGN_SL: return ZEND_SL;
		case ZEND_ASSIGN_SR: return ZEND_SR;
		case ZEND_ASSIGN_CONCAT: return ZEND_CONCAT;
		case ZEND_ASSIGN_BW_OR: return ZEND_BW_OR;
		case ZEND_ASSIGN_BW_AND: return ZEND_BW_AND;
		case ZEND_ASSIGN_BW_XOR: return ZEND_BW_XOR;
		case ZEND_ASSIGN_POW: return ZEND_POW;
		EMPTY_SWITCH_DEFAULT_CASE()
	}
}

static inline zend_bool is_init_opline(zend_op *opline) {
	switch (opline->opcode) {
		case ZEND_INIT_FCALL_BY_NAME:
		case ZEND_INIT_NS_FCALL_BY_NAME:
		case ZEND_NEW:
		case ZEND_INIT_DYNAMIC_CALL:
		case ZEND_INIT_METHOD_CALL:
		case ZEND_INIT_STATIC_METHOD_CALL:
		case ZEND_INIT_FCALL:
		case ZEND_INIT_USER_CALL:
			return 1;
		default:
			return 0;
	}
}

static inline zend_bool is_call_opline(zend_op *opline) {
	switch (opline->opcode) {
		case ZEND_DO_FCALL:
		case ZEND_DO_ICALL:
		case ZEND_DO_UCALL:
		case ZEND_DO_FCALL_BY_NAME:
			return 1;
		default:
			return 0;
	}
}

#endif

