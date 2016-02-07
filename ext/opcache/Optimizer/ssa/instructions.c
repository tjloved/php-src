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
		case ZEND_BW_OR:
		case ZEND_BW_AND:
		case ZEND_BW_XOR:
		case ZEND_CONCAT:
		case ZEND_FAST_CONCAT:
			/* For concat we are discounting the string size overflow error */
			return !MUST_BE(t1, MAY_BE_SIMPLE) || !MUST_BE(t2, MAY_BE_SIMPLE);
		case ZEND_DIV:
		case ZEND_MOD:
			if (!MUST_BE(t1, MAY_BE_SIMPLE) || !MUST_BE(t2, MAY_BE_SIMPLE)) {
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
				// TODO This can be more accurate:
				// If one side is constant or if both sides have limited array value types
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
			/* OB handler */
		case ZEND_DO_FCALL:
		case ZEND_DO_FCALL_BY_NAME:
		case ZEND_DO_ICALL:
		case ZEND_DO_UCALL:
		case ZEND_INCLUDE_OR_EVAL:
		case ZEND_THROW:
			/* Obvious... */
		case ZEND_RECV:
			/* Missing argument or type mismatch */
		case ZEND_EXT_STMT:
		case ZEND_EXT_FCALL_BEGIN:
		case ZEND_EXT_FCALL_END:
		case ZEND_EXT_NOP:
		case ZEND_TICKS:
			/* We don't know what these are going to do */
			return 1;
		case ZEND_RECV_INIT:
			// TODO More accurate: If init is non-constant and there is no type hint
			return 1;
		default:
			return 1;
	}
#define UNSET_VAR (non-quick-set)
#define ZEND_INIT_FCALL_BY_NAME               59
#define ZEND_INIT_FCALL                       61
#define ZEND_RETURN                           62
#define ZEND_SEND_VAL                         65
#define ZEND_SEND_VAR_EX                      66
#define ZEND_SEND_REF                         67
#define ZEND_NEW                              68
#define ZEND_INIT_NS_FCALL_BY_NAME            69
#define ZEND_UNSET_DIM                        75
#define ZEND_UNSET_OBJ                        76
#define ZEND_FE_RESET_R                       77
#define ZEND_FE_FETCH_R                       78
#define ZEND_EXIT                             79
#define ZEND_FETCH_R                          80
#define ZEND_FETCH_DIM_R                      81
#define ZEND_FETCH_OBJ_R                      82
#define ZEND_FETCH_W                          83
#define ZEND_FETCH_DIM_W                      84
#define ZEND_FETCH_OBJ_W                      85
#define ZEND_FETCH_RW                         86
#define ZEND_FETCH_DIM_RW                     87
#define ZEND_FETCH_OBJ_RW                     88
#define ZEND_FETCH_IS                         89
#define ZEND_FETCH_DIM_IS                     90
#define ZEND_FETCH_OBJ_IS                     91
#define ZEND_FETCH_FUNC_ARG                   92
#define ZEND_FETCH_DIM_FUNC_ARG               93
#define ZEND_FETCH_OBJ_FUNC_ARG               94
#define ZEND_FETCH_UNSET                      95
#define ZEND_FETCH_DIM_UNSET                  96
#define ZEND_FETCH_OBJ_UNSET                  97
#define ZEND_FETCH_LIST                       98
#define ZEND_FETCH_CONSTANT                   99
#define ZEND_SEND_VAR_NO_REF                 106
#define ZEND_CATCH                           107
#define ZEND_FETCH_CLASS                     109
#define ZEND_CLONE                           110
#define ZEND_RETURN_BY_REF                   111
#define ZEND_INIT_METHOD_CALL                112
#define ZEND_INIT_STATIC_METHOD_CALL         113
#define ZEND_ISSET_ISEMPTY_VAR               114
#define ZEND_ISSET_ISEMPTY_DIM_OBJ           115
#define ZEND_SEND_VAL_EX                     116
#define ZEND_SEND_VAR                        117
#define ZEND_INIT_USER_CALL                  118
#define ZEND_SEND_ARRAY                      119
#define ZEND_SEND_USER                       120
#define ZEND_STRLEN                          121
#define ZEND_VERIFY_RETURN_TYPE              124
#define ZEND_FE_RESET_RW                     125
#define ZEND_FE_FETCH_RW                     126
#define ZEND_FE_FREE                         127
#define ZEND_INIT_DYNAMIC_CALL               128
#define ZEND_PRE_INC_OBJ                     132
#define ZEND_PRE_DEC_OBJ                     133
#define ZEND_POST_INC_OBJ                    134
#define ZEND_POST_DEC_OBJ                    135
#define ZEND_ASSIGN_OBJ                      136
#define ZEND_OP_DATA                         137
#define ZEND_INSTANCEOF                      138
#define ZEND_DECLARE_CLASS                   139
#define ZEND_DECLARE_INHERITED_CLASS         140
#define ZEND_DECLARE_FUNCTION                141
#define ZEND_YIELD_FROM                      142
#define ZEND_DECLARE_CONST                   143
#define ZEND_ADD_INTERFACE                   144
#define ZEND_DECLARE_INHERITED_CLASS_DELAYED 145
#define ZEND_VERIFY_ABSTRACT_CLASS           146
#define ZEND_ASSIGN_DIM                      147
#define ZEND_ISSET_ISEMPTY_PROP_OBJ          148
#define ZEND_HANDLE_EXCEPTION                149
#define ZEND_USER_OPCODE                     150
#define ZEND_DECLARE_LAMBDA_FUNCTION         153
#define ZEND_ADD_TRAIT                       154
#define ZEND_BIND_TRAITS                     155
#define ZEND_SEPARATE                        156
#define ZEND_FETCH_CLASS_NAME                157
#define ZEND_CALL_TRAMPOLINE                 158
#define ZEND_DISCARD_EXCEPTION               159
#define ZEND_YIELD                           160
#define ZEND_GENERATOR_RETURN                161
#define ZEND_FAST_CALL                       162
#define ZEND_FAST_RET                        163
#define ZEND_RECV_VARIADIC                   164
#define ZEND_SEND_UNPACK                     165
#define ZEND_BIND_GLOBAL                     168
#define ZEND_SPACESHIP                       170
#define ZEND_DECLARE_ANON_CLASS              171
#define ZEND_DECLARE_ANON_INHERITED_CLASS    172
#define ZEND_FETCH_STATIC_PROP_R             173
#define ZEND_FETCH_STATIC_PROP_W             174
#define ZEND_FETCH_STATIC_PROP_RW            175
#define ZEND_FETCH_STATIC_PROP_IS            176
#define ZEND_FETCH_STATIC_PROP_FUNC_ARG      177
#define ZEND_FETCH_STATIC_PROP_UNSET         178
#define ZEND_UNSET_STATIC_PROP               179
#define ZEND_ISSET_ISEMPTY_STATIC_PROP       180
#define ZEND_FETCH_CLASS_CONSTANT            181
}
