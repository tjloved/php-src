#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/instructions.h"

/* Whether the opline might throw an exception or a diagnostic, excluding
 * exceptions thrown by destruction of the opline operands. */
zend_bool may_throw(
		zend_op_array *op_array, zend_ssa *ssa, const zend_op *opline, const zend_ssa_op *ssa_op) {
	uint32_t t1 = OP1_INFO();
	uint32_t t2 = OP2_INFO();

	switch (opline->opcode) {
		case ZEND_UNSET_VAR:
			if (opline->extended_value & ZEND_QUICK_SET) {
				return 0;
			}
			break;
		case ZEND_ASSIGN:
			return t2 & MAY_BE_UNDEF;
	}

	if ((opline->op1_type != IS_UNUSED && (t1 & MAY_BE_UNDEF))
			|| (opline->op2_type != IS_UNUSED && (t2 & MAY_BE_UNDEF))) {
		/* Undefined variables generally throw notices. The exceptions to this
		 * rule are handled above. */
		return 1;
	}

	switch (opline->opcode) {
		case ZEND_NOP:
		case ZEND_IS_IDENTICAL:
		case ZEND_IS_NOT_IDENTICAL:
		case ZEND_QM_ASSIGN:
		case ZEND_JMP:
		case ZEND_FREE:
		case ZEND_BEGIN_SILENCE:
		case ZEND_END_SILENCE:
		case ZEND_COALESCE:
		case ZEND_TYPE_CHECK:
		case ZEND_DEFINED:
			return 0;
		case ZEND_ADD:
			if (CAN_BE(t1, MAY_BE_ARRAY) || CAN_BE(t2, MAY_BE_ARRAY)) {
				return !MUST_BE(t1, MAY_BE_ARRAY) || !MUST_BE(t2, MAY_BE_ARRAY);
			}
			/* break missing intentionally */
		case ZEND_SUB:
		case ZEND_MUL:
		case ZEND_POW:
			if (!MUST_BE(t1, MAY_BE_SIMPLE) || !MUST_BE(t2, MAY_BE_SIMPLE)) {
				return 1;
			}
			return CAN_BE(t1, MAY_BE_STRING) || CAN_BE(t2, MAY_BE_STRING);
		case ZEND_BW_OR:
		case ZEND_BW_AND:
		case ZEND_BW_XOR:
			if (!MUST_BE(t1, MAY_BE_SIMPLE) || !MUST_BE(t2, MAY_BE_SIMPLE)) {
				return 1;
			}
			if (MUST_BE(t1, MAY_BE_STRING) && MUST_BE(t2, MAY_BE_STRING)) {
				return 0;
			}
			return CAN_BE(t1, MAY_BE_STRING) || CAN_BE(t2, MAY_BE_STRING);
		case ZEND_CONCAT:
		case ZEND_FAST_CONCAT:
			/* We are discounting the string size overflow error */
			return !MUST_BE(t1, MAY_BE_SIMPLE) || !MUST_BE(t2, MAY_BE_SIMPLE);
		case ZEND_DIV:
		case ZEND_MOD:
			if (!MUST_BE(t1, MAY_BE_SIMPLE) || !MUST_BE(t2, MAY_BE_SIMPLE)) {
				return 1;
			}
			if (CAN_BE(t1, MAY_BE_STRING) || CAN_BE(t2, MAY_BE_STRING)) {
				return 1;
			}
			if (OP2_HAS_RANGE()) {
				/* Division by zero */
				return OP2_MIN_RANGE() <= 0 && OP2_MAX_RANGE() >= 0;
			}
			return 1;
		case ZEND_BOOL_XOR:
			/* Object to boolean cast can error */
			return CAN_BE(t1, MAY_BE_OBJECT) || CAN_BE(t2, MAY_BE_OBJECT);
		case ZEND_BOOL:
		case ZEND_BOOL_NOT:
		case ZEND_PRE_INC:
		case ZEND_POST_INC:
		case ZEND_PRE_DEC:
		case ZEND_POST_DEC:
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
		case ZEND_JMPZNZ:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_JMP_SET:
			return CAN_BE(t1, MAY_BE_OBJECT);
		case ZEND_BW_NOT:
			return !MUST_BE(t1, MAY_BE_LONG|MAY_BE_DOUBLE|MAY_BE_STRING);
		case ZEND_SL:
		case ZEND_SR:
			if (!MUST_BE(t1, MAY_BE_SIMPLE) || !MUST_BE(t2, MAY_BE_SIMPLE)) {
				return 1;
			}
			if (CAN_BE(t1, MAY_BE_STRING) || CAN_BE(t2, MAY_BE_STRING)) {
				return 1;
			}
			if (OP2_HAS_RANGE()) {
				/* Negative shift */
				return OP2_MIN_RANGE() < 0;
			}
			return 1;
		case ZEND_IS_EQUAL:
		case ZEND_IS_NOT_EQUAL:
		case ZEND_IS_SMALLER:
		case ZEND_IS_SMALLER_OR_EQUAL:
		case ZEND_CASE:
			if (MUST_BE(t1, MAY_BE_NULL) || MUST_BE(t2, MAY_BE_NULL)) {
				/* Comparisons to NULL are always safe, even for objects */
				return 0;
			}

			if (CAN_BE(t1, MAY_BE_OBJECT) || CAN_BE(t2, MAY_BE_OBJECT)) {
				return 1;
			}

			if (CAN_BE(t1, MAY_BE_ARRAY) || CAN_BE(t2, MAY_BE_ARRAY)) {
				if (opline->op1_type == IS_CONST || opline->op2_type == IS_CONST) {
					/* If one side is constant, we cannot run into a recursive case */
					return 0;
				}
				/* There may be a recursion warning */
				return 1;
			}
			return 0;
		case ZEND_CAST:
			switch (opline->extended_value) {
				case IS_NULL:
				case IS_OBJECT:
					return 0;
				case _IS_BOOL:
				case IS_LONG:
				case IS_DOUBLE:
				case IS_ARRAY:
					return CAN_BE(t1, MAY_BE_OBJECT);
				case IS_STRING:
					return !MUST_BE(t1, MAY_BE_SIMPLE);
				EMPTY_SWITCH_DEFAULT_CASE()
			}
			return 0;
		case ZEND_ROPE_INIT:
		case ZEND_ROPE_ADD:
		case ZEND_ROPE_END:
			return !MUST_BE(t1, MAY_BE_SIMPLE);
		case ZEND_INIT_ARRAY:
		case ZEND_ADD_ARRAY_ELEMENT:
			return opline->op2_type != IS_UNUSED && !MUST_BE(t2, MAY_BE_SIMPLE);
		case ZEND_ASSIGN_ADD:
		case ZEND_ASSIGN_SUB:
		case ZEND_ASSIGN_MUL:
		case ZEND_ASSIGN_DIV:
		case ZEND_ASSIGN_MOD:
		case ZEND_ASSIGN_SL:
		case ZEND_ASSIGN_SR:
		case ZEND_ASSIGN_CONCAT:
		case ZEND_ASSIGN_BW_OR:
		case ZEND_ASSIGN_BW_AND:
		case ZEND_ASSIGN_BW_XOR:
		case ZEND_ASSIGN_POW:
			// TODO Reduce these to their primitives
			return 1;
		case ZEND_ECHO:
		case ZEND_DO_FCALL:
		case ZEND_DO_FCALL_BY_NAME:
		case ZEND_DO_ICALL:
		case ZEND_DO_UCALL:
		case ZEND_INCLUDE_OR_EVAL:
		case ZEND_THROW:
		case ZEND_EXT_STMT:
		case ZEND_EXT_FCALL_BEGIN:
		case ZEND_EXT_FCALL_END:
		case ZEND_EXT_NOP:
		case ZEND_TICKS:
		case ZEND_YIELD:
		case ZEND_YIELD_FROM:
		case ZEND_RECV:
		case ZEND_RECV_INIT:
			/* Even if more accurate information were possible, it is not wortwhile as they are
			 * always treated as having side-effects anyway. */
			return 1;
		default:
			return 1;
	}

#if 0
UNSET_VAR (non-quick-set)
ZEND_INIT_FCALL_BY_NAME
ZEND_INIT_FCALL
ZEND_RETURN
ZEND_SEND_VAL
ZEND_SEND_VAR_EX
ZEND_SEND_REF
ZEND_NEW
ZEND_INIT_NS_FCALL_BY_NAME
ZEND_UNSET_DIM
ZEND_UNSET_OBJ
ZEND_FE_RESET_R
ZEND_FE_FETCH_R
ZEND_EXIT
ZEND_FETCH_R
ZEND_FETCH_DIM_R
ZEND_FETCH_OBJ_R
ZEND_FETCH_W
ZEND_FETCH_DIM_W
ZEND_FETCH_OBJ_W
ZEND_FETCH_RW
ZEND_FETCH_DIM_RW
ZEND_FETCH_OBJ_RW
ZEND_FETCH_IS
ZEND_FETCH_DIM_IS
ZEND_FETCH_OBJ_IS
ZEND_FETCH_FUNC_ARG
ZEND_FETCH_DIM_FUNC_ARG
ZEND_FETCH_OBJ_FUNC_ARG
ZEND_FETCH_UNSET
ZEND_FETCH_DIM_UNSET
ZEND_FETCH_OBJ_UNSET
ZEND_FETCH_LIST
ZEND_FETCH_CONSTANT
ZEND_SEND_VAR_NO_REF
ZEND_CATCH
ZEND_FETCH_CLASS
ZEND_CLONE
ZEND_RETURN_BY_REF
ZEND_INIT_METHOD_CALL
ZEND_INIT_STATIC_METHOD_CALL
ZEND_ISSET_ISEMPTY_VAR
ZEND_ISSET_ISEMPTY_DIM_OBJ
ZEND_SEND_VAL_EX
ZEND_SEND_VAR
ZEND_INIT_USER_CALL
ZEND_SEND_ARRAY
ZEND_SEND_USER
ZEND_STRLEN
ZEND_VERIFY_RETURN_TYPE
ZEND_FE_RESET_RW
ZEND_FE_FETCH_RW
ZEND_FE_FREE
ZEND_INIT_DYNAMIC_CALL
ZEND_PRE_INC_OBJ
ZEND_PRE_DEC_OBJ
ZEND_POST_INC_OBJ
ZEND_POST_DEC_OBJ
ZEND_ASSIGN_OBJ
ZEND_OP_DATA
ZEND_INSTANCEOF
ZEND_DECLARE_CLASS
ZEND_DECLARE_INHERITED_CLASS
ZEND_DECLARE_FUNCTION
ZEND_DECLARE_CONST
ZEND_ADD_INTERFACE
ZEND_DECLARE_INHERITED_CLASS_DELAYED
ZEND_VERIFY_ABSTRACT_CLASS
ZEND_ASSIGN_DIM
ZEND_ISSET_ISEMPTY_PROP_OBJ
ZEND_HANDLE_EXCEPTION
ZEND_USER_OPCODE
ZEND_DECLARE_LAMBDA_FUNCTION
ZEND_ADD_TRAIT
ZEND_BIND_TRAITS
ZEND_SEPARATE
ZEND_FETCH_CLASS_NAME
ZEND_CALL_TRAMPOLINE
ZEND_DISCARD_EXCEPTION
ZEND_GENERATOR_RETURN
ZEND_FAST_CALL
ZEND_FAST_RET
ZEND_RECV_VARIADIC
ZEND_SEND_UNPACK
ZEND_BIND_GLOBAL
ZEND_SPACESHIP
ZEND_DECLARE_ANON_CLASS
ZEND_DECLARE_ANON_INHERITED_CLASS
ZEND_FETCH_STATIC_PROP_R
ZEND_FETCH_STATIC_PROP_W
ZEND_FETCH_STATIC_PROP_RW
ZEND_FETCH_STATIC_PROP_IS
ZEND_FETCH_STATIC_PROP_FUNC_ARG
ZEND_FETCH_STATIC_PROP_UNSET
ZEND_UNSET_STATIC_PROP
ZEND_ISSET_ISEMPTY_STATIC_PROP
ZEND_FETCH_CLASS_CONSTANT
#endif
}
