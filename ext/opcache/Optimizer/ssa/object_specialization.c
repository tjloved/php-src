#include "ZendAccelerator.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/ssa_pass.h"
#include "Optimizer/ssa/instructions.h"

/* This pass tries to replace FETCH_OBJ_R opcodes with FETCH_OBJ_R_FIXED opcodes, which is
 * hardwired to a certain property offset.
 *
 * TODO: Support the same for ASSIGN_OBJ
 */

static inline zend_bool is_property_accessible(
		zend_property_info *property_info, zend_class_entry *ce, zend_class_entry *scope) {
	if (property_info->flags & ZEND_ACC_PUBLIC) {
		return 1;
	} else if (property_info->flags & ZEND_ACC_PRIVATE) {
		return ce == scope || property_info->ce == scope;
	} else if (property_info->flags & ZEND_ACC_PROTECTED) {
		return zend_check_protected(property_info->ce, scope);
	}
	return 0;
}

static uint32_t get_property_offset(
		zend_string *member, zend_class_entry *ce, zend_class_entry *scope) {
	zend_property_info *property_info;
	if (zend_hash_num_elements(&ce->properties_info) == 0) {
		return -1;
	}

	property_info = zend_hash_find_ptr(&ce->properties_info, member);
	if (property_info) {
		uint32_t flags = property_info->flags;
		if (flags & ZEND_ACC_STATIC) {
			return -1;
		}
		if (!(flags & ZEND_ACC_SHADOW) && is_property_accessible(property_info, ce, scope)) {
			if (!(flags & ZEND_ACC_CHANGED) || (flags & ZEND_ACC_PRIVATE)) {
				return property_info->offset;
			}
		} else {
			/* Shadow property or inaccessible, try the scope */
			property_info = NULL;
		}
	}

	if (scope && scope != ce && instanceof_function(ce, scope)) {
		zend_property_info *maybe_property_info =
			zend_hash_find_ptr(&scope->properties_info, member);
		if (maybe_property_info && maybe_property_info->flags & ZEND_ACC_PRIVATE) {
			property_info = maybe_property_info;
			if (property_info->flags & ZEND_ACC_STATIC) {
				return -1;
			}
		}
	}
	
	return property_info ? property_info->offset : -1;
}

/* Checks if the property offsets of a class are finalized. This handles the case where a class
 * uses delayed binding but still has finalized offsets because it only implements interfaces. */
static zend_bool has_finalized_property_offsets(zend_script *script, zend_string *lcname) {
	zend_op_array *op_array = &script->main_op_array;
	int i;
	if (zend_hash_exists(&script->class_table, lcname)) {
		return 1;
	}

	// TODO Cache this, or solve it better altogether
	// TODO Duplicate conditional classes?
	for (i = 0; i < op_array->last; i++) {
		zend_op *opline = &op_array->opcodes[i];
		zval *op1;
		switch (opline->opcode) {
			case ZEND_DECLARE_CLASS:
			case ZEND_DECLARE_ANON_CLASS:
				op1 = RT_CONSTANT(op_array, opline->op1);
				if (zend_string_equals_literal(Z_STR_P(op1), lcname)) {
					return 1;
				}
				break;
			case ZEND_DECLARE_INHERITED_CLASS:
			case ZEND_DECLARE_INHERITED_CLASS_DELAYED:
			case ZEND_DECLARE_ANON_INHERITED_CLASS:
				op1 = RT_CONSTANT(op_array, opline->op1);
				if (zend_string_equals_literal(Z_STR_P(op1), lcname)) {
					return 0;
				}
				break;
		}
	}

	/* Dynamic class, declared elsewhere */
	return 0;
}

void ssa_optimize_object_specialization(ssa_opt_ctx *ctx) {
	zend_op_array *op_array = ctx->op_array;
	zend_ssa *ssa = ctx->ssa;
	int i;
	FOREACH_INSTR_NUM(i) {
		zend_op *opline = &op_array->opcodes[i];
		zend_ssa_op *ssa_op = &ssa->ops[i];
		uint32_t t1 = OP1_INFO();
		uint32_t t2 = OP2_INFO();

		/* Skip instructions with potentially undefined CVs */
		if ((opline->op1_type != IS_UNUSED && (t1 & MAY_BE_UNDEF))
				|| (opline->op2_type != IS_UNUSED && (t2 & MAY_BE_UNDEF))) {
			continue;
		}

		switch (opline->opcode) {
			case ZEND_FETCH_OBJ_R:
			{
				/* Converts to FETCH_OBJ_R_FIXED with a fixed property offset, if possible */
				zend_class_entry *ce;
				zend_string *member;
				uint32_t offset;
				if (opline->op2_type != IS_CONST) {
					break;
				}
				if (opline->op1_type == IS_UNUSED) {
					zend_string *lcname;
					if (!op_array->scope) {
						/* $this in changeable scope */
						break;
					}

					ce = op_array->scope;
					if (ce->ce_flags & ZEND_ACC_TRAIT) {
						/* Traits have changeable scope. */
						break;
					}

					lcname = zend_string_tolower(ce->name);
					if (!has_finalized_property_offsets(ctx->opt_ctx->script, lcname)) {
						zend_string_release(lcname);
						break;
					}
					zend_string_release(lcname);
				} else {
					zend_ssa_var_info *info = &ssa->var_info[ssa_op->op1_use];
					if (!MUST_BE(t1, MAY_BE_OBJECT) || !info->ce) {
						break;
					}
					ce = info->ce;
				}
				if (ce->type == ZEND_INTERNAL_CLASS) {
					/* Often don't use ordinary properties */
					break;
				}

				member = Z_STR(ZEND_OP2_LITERAL(opline));
				offset = get_property_offset(member, ce, op_array->scope);
				if (offset == (uint32_t) -1) {
					break;
				}

				opline->opcode = ZEND_FETCH_OBJ_R_FIXED;
				opline->extended_value = offset;
				break;
			}
		}
	} FOREACH_INSTR_NUM_END();
}
