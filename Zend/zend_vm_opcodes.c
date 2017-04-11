/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2017 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include <zend.h>

static const char *zend_vm_opcodes_names[214] = {
	"ZEND_NOP",
	"ZEND_ADD",
	"ZEND_SUB",
	"ZEND_MUL",
	"ZEND_DIV",
	"ZEND_MOD",
	"ZEND_SL",
	"ZEND_SR",
	"ZEND_CONCAT",
	"ZEND_BW_OR",
	"ZEND_BW_AND",
	"ZEND_BW_XOR",
	"ZEND_BW_NOT",
	"ZEND_BOOL_NOT",
	"ZEND_BOOL_XOR",
	"ZEND_IS_IDENTICAL",
	"ZEND_IS_NOT_IDENTICAL",
	"ZEND_IS_EQUAL",
	"ZEND_IS_NOT_EQUAL",
	"ZEND_IS_SMALLER",
	"ZEND_IS_SMALLER_OR_EQUAL",
	"ZEND_CAST",
	"ZEND_QM_ASSIGN",
	"ZEND_ASSIGN_ADD",
	"ZEND_ASSIGN_SUB",
	"ZEND_ASSIGN_MUL",
	"ZEND_ASSIGN_DIV",
	"ZEND_ASSIGN_MOD",
	"ZEND_ASSIGN_SL",
	"ZEND_ASSIGN_SR",
	"ZEND_ASSIGN_CONCAT",
	"ZEND_ASSIGN_BW_OR",
	"ZEND_ASSIGN_BW_AND",
	"ZEND_ASSIGN_BW_XOR",
	"ZEND_PRE_INC",
	"ZEND_PRE_DEC",
	"ZEND_POST_INC",
	"ZEND_POST_DEC",
	"ZEND_ASSIGN",
	"ZEND_ASSIGN_REF",
	"ZEND_ECHO",
	"ZEND_GENERATOR_CREATE",
	"ZEND_JMP",
	"ZEND_JMPZ",
	"ZEND_JMPNZ",
	"ZEND_JMPZNZ",
	"ZEND_JMPZ_EX",
	"ZEND_JMPNZ_EX",
	"ZEND_CASE",
	"ZEND_CHECK_VAR",
	"ZEND_SEND_VAR_NO_REF_EX",
	"ZEND_MAKE_REF",
	"ZEND_BOOL",
	"ZEND_FAST_CONCAT",
	"ZEND_ROPE_INIT",
	"ZEND_ROPE_ADD",
	"ZEND_ROPE_END",
	"ZEND_BEGIN_SILENCE",
	"ZEND_END_SILENCE",
	"ZEND_INIT_FCALL_BY_NAME",
	"ZEND_DO_FCALL",
	"ZEND_INIT_FCALL",
	"ZEND_RETURN",
	"ZEND_RECV",
	"ZEND_RECV_INIT",
	"ZEND_SEND_VAL",
	"ZEND_SEND_VAR_EX",
	"ZEND_SEND_REF",
	"ZEND_NEW",
	"ZEND_INIT_NS_FCALL_BY_NAME",
	"ZEND_FREE",
	"ZEND_INIT_ARRAY",
	"ZEND_ADD_ARRAY_ELEMENT",
	"ZEND_INCLUDE_OR_EVAL",
	"ZEND_UNSET_VAR",
	"ZEND_UNSET_DIM",
	"ZEND_UNSET_OBJ",
	"ZEND_FE_RESET_R",
	"ZEND_FE_FETCH_R",
	"ZEND_EXIT",
	"ZEND_FETCH_R",
	"ZEND_FETCH_DIM_R",
	"ZEND_FETCH_OBJ_R",
	"ZEND_FETCH_W",
	"ZEND_FETCH_DIM_W",
	"ZEND_FETCH_OBJ_W",
	"ZEND_FETCH_RW",
	"ZEND_FETCH_DIM_RW",
	"ZEND_FETCH_OBJ_RW",
	"ZEND_FETCH_IS",
	"ZEND_FETCH_DIM_IS",
	"ZEND_FETCH_OBJ_IS",
	"ZEND_FETCH_FUNC_ARG",
	"ZEND_FETCH_DIM_FUNC_ARG",
	"ZEND_FETCH_OBJ_FUNC_ARG",
	"ZEND_FETCH_UNSET",
	"ZEND_FETCH_DIM_UNSET",
	"ZEND_FETCH_OBJ_UNSET",
	"ZEND_FETCH_LIST",
	"ZEND_FETCH_CONSTANT",
	NULL,
	"ZEND_EXT_STMT",
	"ZEND_EXT_FCALL_BEGIN",
	"ZEND_EXT_FCALL_END",
	"ZEND_EXT_NOP",
	"ZEND_TICKS",
	"ZEND_SEND_VAR_NO_REF",
	"ZEND_CATCH",
	"ZEND_THROW",
	"ZEND_FETCH_CLASS",
	"ZEND_CLONE",
	"ZEND_RETURN_BY_REF",
	"ZEND_INIT_METHOD_CALL",
	"ZEND_INIT_STATIC_METHOD_CALL",
	"ZEND_ISSET_ISEMPTY_VAR",
	"ZEND_ISSET_ISEMPTY_DIM_OBJ",
	"ZEND_SEND_VAL_EX",
	"ZEND_SEND_VAR",
	"ZEND_INIT_USER_CALL",
	"ZEND_SEND_ARRAY",
	"ZEND_SEND_USER",
	"ZEND_STRLEN",
	"ZEND_DEFINED",
	"ZEND_TYPE_CHECK",
	"ZEND_VERIFY_RETURN_TYPE",
	"ZEND_FE_RESET_RW",
	"ZEND_FE_FETCH_RW",
	"ZEND_FE_FREE",
	"ZEND_INIT_DYNAMIC_CALL",
	"ZEND_DO_ICALL",
	"ZEND_DO_UCALL",
	"ZEND_DO_FCALL_BY_NAME",
	"ZEND_PRE_INC_OBJ",
	"ZEND_PRE_DEC_OBJ",
	"ZEND_POST_INC_OBJ",
	"ZEND_POST_DEC_OBJ",
	"ZEND_ASSIGN_OBJ",
	"ZEND_OP_DATA",
	"ZEND_INSTANCEOF",
	"ZEND_DECLARE_CLASS",
	"ZEND_DECLARE_INHERITED_CLASS",
	"ZEND_DECLARE_FUNCTION",
	"ZEND_YIELD_FROM",
	"ZEND_DECLARE_CONST",
	"ZEND_ADD_INTERFACE",
	"ZEND_DECLARE_INHERITED_CLASS_DELAYED",
	"ZEND_VERIFY_ABSTRACT_CLASS",
	"ZEND_ASSIGN_DIM",
	"ZEND_ISSET_ISEMPTY_PROP_OBJ",
	"ZEND_HANDLE_EXCEPTION",
	"ZEND_USER_OPCODE",
	"ZEND_ASSERT_CHECK",
	"ZEND_JMP_SET",
	"ZEND_DECLARE_LAMBDA_FUNCTION",
	"ZEND_ADD_TRAIT",
	"ZEND_BIND_TRAITS",
	"ZEND_SEPARATE",
	"ZEND_FETCH_CLASS_NAME",
	"ZEND_CALL_TRAMPOLINE",
	"ZEND_DISCARD_EXCEPTION",
	"ZEND_YIELD",
	"ZEND_GENERATOR_RETURN",
	"ZEND_FAST_CALL",
	"ZEND_FAST_RET",
	"ZEND_RECV_VARIADIC",
	"ZEND_SEND_UNPACK",
	"ZEND_POW",
	"ZEND_ASSIGN_POW",
	"ZEND_BIND_GLOBAL",
	"ZEND_COALESCE",
	"ZEND_SPACESHIP",
	"ZEND_DECLARE_ANON_CLASS",
	"ZEND_DECLARE_ANON_INHERITED_CLASS",
	"ZEND_FETCH_STATIC_PROP_R",
	"ZEND_FETCH_STATIC_PROP_W",
	"ZEND_FETCH_STATIC_PROP_RW",
	"ZEND_FETCH_STATIC_PROP_IS",
	"ZEND_FETCH_STATIC_PROP_FUNC_ARG",
	"ZEND_FETCH_STATIC_PROP_UNSET",
	"ZEND_UNSET_STATIC_PROP",
	"ZEND_ISSET_ISEMPTY_STATIC_PROP",
	"ZEND_FETCH_CLASS_CONSTANT",
	"ZEND_BIND_LEXICAL",
	"ZEND_BIND_STATIC",
	"ZEND_FETCH_THIS",
	NULL,
	"ZEND_ISSET_ISEMPTY_THIS",
	"ZEND_SWITCH_LONG",
	"ZEND_SWITCH_STRING",
	NULL,
	"ZEND_PHI_ASSIGN",
	"ZEND_IS_SMALLER_FLOAT",
	"ZEND_IS_SMALLER_INT",
	"ZEND_ADD_INT_NO_OVERFLOW",
	"ZEND_FETCH_DIM_INT",
	"ZEND_MOV_UNCOUNTED",
	"ZEND_DIV_FLOAT",
	"ZEND_ENSURE_HAVE_THIS",
	"ZEND_ASSERT_TYPE",
	NULL,
	"ZEND_ADD_INT",
	"ZEND_SUB_INT",
	"ZEND_ADD_FLOAT",
	"ZEND_SUB_FLOAT",
	"ZEND_MUL_FLOAT",
	"ZEND_FETCH_OBJ_R_FIXED",
	"ZEND_FETCH_DIM_INT_W",
	"ZEND_POST_INC_INT",
	"ZEND_POST_DEC_INT",
	"ZEND_RECV_INIT_FAST",
	"ZEND_SEND_VAR_FAST",
	"ZEND_PRE_INC_INT",
	"ZEND_PRE_INC_INT_NO_OVERFLOW",
	"ZEND_PRE_DEC_INT",
};

static uint32_t zend_vm_opcodes_flags[214] = {
	0x00000000,
	0x00000707,
	0x00000707,
	0x00000707,
	0x00000707,
	0x00000707,
	0x00000707,
	0x00000707,
	0x00000707,
	0x00000707,
	0x00000707,
	0x00000707,
	0x00000007,
	0x00000007,
	0x00000707,
	0x00000303,
	0x00000303,
	0x00000707,
	0x00000707,
	0x00000707,
	0x00000707,
	0x07000003,
	0x00000003,
	0x04006751,
	0x04006751,
	0x04006751,
	0x04006751,
	0x04006751,
	0x04006751,
	0x04006751,
	0x04006751,
	0x04006751,
	0x04006751,
	0x04006751,
	0x00000001,
	0x00000001,
	0x00000001,
	0x00000001,
	0x00000301,
	0x0b000101,
	0x00000007,
	0x00000000,
	0x00000020,
	0x00002007,
	0x00002007,
	0x03002007,
	0x00002007,
	0x00002007,
	0x00000707,
	0x00000101,
	0x00001001,
	0x00000101,
	0x00000007,
	0x00000707,
	0x01000701,
	0x01000701,
	0x01000701,
	0x00000000,
	0x00000001,
	0x01000300,
	0x00000000,
	0x01000310,
	0x00000003,
	0x00000010,
	0x00000310,
	0x00001007,
	0x00001001,
	0x00001001,
	0x01000073,
	0x01000300,
	0x00004005,
	0x00186703,
	0x00106703,
	0x08000007,
	0x00030107,
	0x00000701,
	0x00000751,
	0x00002003,
	0x03000001,
	0x00000007,
	0x00010107,
	0x00000707,
	0x00000753,
	0x00010107,
	0x00006701,
	0x00000751,
	0x00010107,
	0x00006701,
	0x00000751,
	0x00010107,
	0x00000707,
	0x00000757,
	0x00050107,
	0x01006703,
	0x01000753,
	0x00010107,
	0x00000701,
	0x00000751,
	0x00000707,
	0x06000301,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x01000000,
	0x00001001,
	0x03000103,
	0x00000003,
	0x05000700,
	0x00000057,
	0x0b000003,
	0x01000757,
	0x01008773,
	0x00030107,
	0x00020707,
	0x00001003,
	0x00001001,
	0x01000703,
	0x00000000,
	0x00001003,
	0x00000007,
	0x00000003,
	0x07000003,
	0x00000103,
	0x00002003,
	0x03000001,
	0x00004005,
	0x01000700,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000751,
	0x00000751,
	0x00000751,
	0x00000751,
	0x00000751,
	0x00000000,
	0x00007305,
	0x00000000,
	0x00000100,
	0x00000000,
	0x00000003,
	0x00000303,
	0x00000300,
	0x00000100,
	0x00000000,
	0x00006701,
	0x00020757,
	0x00000000,
	0x00000000,
	0x00002000,
	0x00002003,
	0x00000103,
	0x00000000,
	0x00000000,
	0x00000101,
	0x05000000,
	0x00000000,
	0x00000000,
	0x0b000303,
	0x00000003,
	0x00000020,
	0x00003000,
	0x00000010,
	0x00000000,
	0x00000707,
	0x04006751,
	0x00000301,
	0x00002003,
	0x00000707,
	0x03000000,
	0x03000100,
	0x00007307,
	0x00007307,
	0x00007307,
	0x00007307,
	0x01007307,
	0x00007307,
	0x00007307,
	0x00027307,
	0x00000373,
	0x00100101,
	0x00100301,
	0x00000101,
	0x00000000,
	0x00000101,
	0x03000307,
	0x03000307,
	0x00000000,
	0x00000101,
	0x40000b0b,
	0x40000b0b,
	0x40000b0b,
	0x00000b05,
	0x0000000b,
	0x40000b0b,
	0x00000000,
	0x00000009,
	0x00000000,
	0x40000b0b,
	0x40000b0b,
	0x40000b0b,
	0x40000b0b,
	0x40000b0b,
	0x01000355,
	0x00000b01,
	0x00000009,
	0x00000009,
	0x00000310,
	0x00001001,
	0x00000009,
	0x00000009,
	0x00000009,
};

ZEND_API const char* zend_get_opcode_name(zend_uchar opcode) {
	return zend_vm_opcodes_names[opcode];
}
ZEND_API uint32_t zend_get_opcode_flags(zend_uchar opcode) {
	return zend_vm_opcodes_flags[opcode];
}
