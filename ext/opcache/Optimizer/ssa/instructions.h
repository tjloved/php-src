#ifndef SSA_INSTRUCTIONS_H
#define SSA_INSTRUCTIONS_H

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

#endif

