#include "Optimizer/zend_optimizer.h"
#include "Optimizer/zend_optimizer_internal.h"
#include "Optimizer/statistics.h"
#include "php.h"

static zend_bool has_illegal_opcodes(const zend_op_array *op_array) {
	const zend_op *opline = op_array->opcodes;
	const zend_op *end = opline + op_array->last;
	for (; opline < end; opline++) {
		switch (opline->opcode) {
			case ZEND_INIT_FCALL:
			case ZEND_INIT_NS_FCALL_BY_NAME:
			{
				zval *zv = RT_CONSTANT(op_array, opline->op2);
				uint32_t type;
				if (opline->opcode == ZEND_INIT_NS_FCALL_BY_NAME) {
					/* The third literal is the lowercased unqualified name */
					zv += 2;
				}

				type = zend_optimizer_classify_function(Z_STR_P(zv), opline->extended_value);
				if (type == ZEND_FUNC_VARARG) {
					/* Uses func_get_args() or similar */
					return 1;
				}
			}
		}
	}

	return 0;
}

static zend_bool can_clone(zend_call_info *call_info) {
	zend_function *func = call_info->callee_func;
	uint32_t i;

	if (func->type == ZEND_INTERNAL_FUNCTION) {
		/* Can't clone internal function */
		return 0;
	}

	if (func->common.fn_flags & (ZEND_ACC_VARIADIC|ZEND_ACC_HAS_TYPE_HINTS)) {
		// TODO Should be able to support this
		return 0;
	}

	if (func->op_array.static_variables) {
		// TODO Would have to handle static variable sharing correctly for this
		return 0;
	}

	if (call_info->caller_init_opline->opcode != ZEND_INIT_FCALL) {
		// TODO For now only support simple calls
		return 0;
	}

	if (call_info->num_args < func->common.required_num_args) {
		/* Too few arguments */
		return 0;
	}

	if (call_info->num_args > func->common.num_args) {
		// TODO In principle we could support more arguments
		return 0;
	}

	for (i = 0; i < call_info->num_args; i++) {
		if (call_info->arg_info[i].opline == NULL) {
			/* SEND_USER, SEND_ARRAY or SEND_UNPACK has been used */
			// TODO It should be possible to support SEND_USER
			return 0;
		}
	}

	for (i = call_info->num_args; i < func->common.num_args; i++) {
		zend_op *recv_opline = &func->op_array.opcodes[i];
		ZEND_ASSERT(recv_opline->opcode == ZEND_RECV_INIT);
		if (Z_CONSTANT_P(RT_CONSTANT(&func->op_array, recv_opline->op2))) {
			/* Default value is constant expression */
			return 0;
		}
	}

	if (has_illegal_opcodes(&func->op_array)) {
		return 0;
	}

	return 1;
}

static uint32_t call_num_constant_args(zend_call_info *call_info) {
	zend_op_array *op_array = &call_info->callee_func->op_array;
	uint32_t i, count = 0;

	/* Number of constant arguments explicitly passed */
	for (i = 0; i < call_info->num_args; i++) {
		zend_op *send_opline = call_info->arg_info[i].opline;
		if (send_opline->opcode == ZEND_SEND_VAL && send_opline->op1_type == IS_CONST) {
			count++;
		}
	}

	/* Number of implicit constant argumnets due to default values */
	if (op_array->num_args > call_info->num_args) {
		count += op_array->num_args - call_info->num_args;
	}

	return count;
}

typedef struct _cloned_func_entry {
	struct _cloned_func_entry *next;
	zend_string *new_name;
	zend_op_array *orig_op_array;
	zend_op_array *cloned_op_array;
	uint32_t num_args;
	uint32_t num_constant_args;
	zval args[1];
} cloned_func_entry;

static cloned_func_entry *create_cloned_func_entry(
		zend_arena **arena, zend_call_info *info, uint32_t num_constant_args) {
	zend_op_array *op_array = &info->callee_func->op_array;
	uint32_t i, num_args = op_array->num_args;
	size_t size = sizeof(cloned_func_entry) + sizeof(zval) * (num_args - 1);
	cloned_func_entry *entry = zend_arena_alloc(arena, size);

	entry->next = NULL;
	entry->orig_op_array = op_array;
	entry->cloned_op_array = NULL;
	entry->num_args = num_args;
	entry->num_constant_args = num_constant_args;

	for (i = 0; i < info->num_args; i++) {
		zend_op *send_opline = info->arg_info[i].opline;
		if (send_opline->opcode == ZEND_SEND_VAL
				&& send_opline->op1_type == IS_CONST) {
			zval *value = RT_CONSTANT(info->caller_op_array, send_opline->op1);
			ZVAL_COPY_VALUE(&entry->args[i], value);
		} else {
			ZVAL_UNDEF(&entry->args[i]);
		}
	}

	for (i = info->num_args; i < op_array->num_args; i++) {
		zend_op *recv_opline = &op_array->opcodes[i];
		ZEND_ASSERT(recv_opline->opcode == ZEND_RECV_INIT);
		ZVAL_COPY_VALUE(&entry->args[i], RT_CONSTANT(op_array, recv_opline->op2));
	}

	return entry;
}

static zend_bool are_args_identical(cloned_func_entry *entry1, cloned_func_entry *entry2) {
	uint32_t i;
	ZEND_ASSERT(entry1->num_args == entry2->num_args);
	for (i = 0; i < entry1->num_args; i++) {
		if (!fast_is_identical_function(&entry1->args[i], &entry2->args[i])) {
			return 0;
		}
	}
	return 1;
}

static cloned_func_entry *find_cloned_func_entry(
		cloned_func_entry *head, cloned_func_entry *entry) {
	cloned_func_entry *current;
	for (current = head; current; current = current->next) {
		if (current->orig_op_array == entry->orig_op_array
				&& are_args_identical(current, entry)) {
			return current;
		}
	}
	return NULL;
}

static void copy_instr(
		zend_op *new_opline, const zend_op *old_opline, const uint32_t *cv_map) {
	*new_opline = *old_opline;

	/* Remap CV offsets */
	if (new_opline->op1_type == IS_CV) {
		new_opline->op1.num = NUM_VAR(cv_map[VAR_NUM(new_opline->op1.num)]);
	}
	if (new_opline->op2_type == IS_CV) {
		new_opline->op2.num = NUM_VAR(cv_map[VAR_NUM(new_opline->op2.num)]);
	}
	if (new_opline->result_type == IS_CV) {
		new_opline->result.num = NUM_VAR(cv_map[VAR_NUM(new_opline->result.num)]);
	}

	/* Map JMP offsets into new op_array */
	switch (new_opline->opcode) {
		case ZEND_JMP:
		case ZEND_FAST_CALL:
			ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op1,
				ZEND_OP1_JMP_ADDR(old_opline) - old_opline + new_opline);
			break;
		case ZEND_JMPZNZ:
		case ZEND_JMPZ:
		case ZEND_JMPNZ:
		case ZEND_JMPZ_EX:
		case ZEND_JMPNZ_EX:
		case ZEND_FE_RESET_R:
		case ZEND_FE_RESET_RW:
		case ZEND_JMP_SET:
		case ZEND_COALESCE:
		case ZEND_ASSERT_CHECK:
			ZEND_SET_OP_JMP_ADDR(new_opline, new_opline->op2,
				ZEND_OP2_JMP_ADDR(old_opline) - old_opline + new_opline);
			break;
		case ZEND_CATCH:
		case ZEND_DECLARE_ANON_CLASS:
		case ZEND_DECLARE_ANON_INHERITED_CLASS:
		case ZEND_FE_FETCH_R:
		case ZEND_FE_FETCH_RW:
			/* Offsets stay the same */
			break;
	} 
}

static void clone_opcodes(
		zend_op *new_opcodes,
		cloned_func_entry *entry, const uint32_t *cv_map) {
	const zend_op_array *old_op_array = entry->orig_op_array;
	zend_op_array *new_op_array = entry->cloned_op_array;
	const zend_op *old_opcodes = old_op_array->opcodes;
	uint32_t i, j;

	j = 0;
	for (i = 0; i < old_op_array->num_args; i++) {
		if (!Z_ISUNDEF(entry->args[i])) {
			continue;
		}

		copy_instr(&new_opcodes[j], &old_opcodes[i], cv_map);
		new_opcodes[j].op1.num = j + 1;
		j++;
	}
	ZEND_ASSERT(j == new_op_array->num_args);

	for (i = 0; i < old_op_array->num_args; i++) {
		zend_op *opline;
		if (Z_ISUNDEF(entry->args[i])) {
			continue;
		}

		opline = &new_opcodes[j];
		opline->opcode = ZEND_ASSIGN;
		opline->op1_type = IS_CV;
		opline->op1.var = NUM_VAR(cv_map[i]);
		opline->result_type = IS_UNUSED;
		opline->extended_value = 0;
		opline->lineno = old_opcodes[i].lineno;

		opline->op2_type = IS_CONST;
		zval_copy_ctor(&entry->args[i]);
		opline->op2.constant = zend_optimizer_add_literal(new_op_array, &entry->args[i]);
		ZEND_PASS_TWO_UPDATE_CONSTANT(new_op_array, opline->op2);
		j++;
	}
	ZEND_ASSERT(j == old_op_array->num_args);

	for (i = old_op_array->num_args; i < old_op_array->last; i++) {
		copy_instr(&new_opcodes[i], &old_opcodes[i], cv_map);
	}
}

static void copy_arg_info(zend_arg_info *target, const zend_arg_info *source) {
	*target = *source;
	if (target->name) {
		zend_string_addref(target->name);
	}
	if (ZEND_TYPE_IS_CLASS(target->type)) {
		zend_string_addref(ZEND_TYPE_NAME(target->type));
	}
}

static zend_arg_info *clone_arg_info(
		const cloned_func_entry *entry, const zend_op_array *old_op_array) {
	zend_bool has_return_type = (old_op_array->fn_flags & ZEND_ACC_HAS_RETURN_TYPE) != 0;
	uint32_t new_num_args = entry->num_args - entry->num_constant_args;
	zend_arg_info *new_arg_info =
		emalloc(sizeof(zend_arg_info) * (new_num_args + has_return_type));
	uint32_t i, j;

	if (has_return_type) {
		copy_arg_info(new_arg_info, &old_op_array->arg_info[-1]);
		new_arg_info++;
	}

	/* Copy arg_info of non-constant parameters */
	j = 0;
	for (i = 0; i < old_op_array->num_args; i++) {
		if (!Z_ISUNDEF(entry->args[i])) {
			continue;
		}

		copy_arg_info(&new_arg_info[j], &old_op_array->arg_info[i]);
		j++;
	}
	ZEND_ASSERT(j == new_num_args);

	return new_arg_info;
}

static void clone_function(cloned_func_entry *entry) {
	uint32_t new_num_args = entry->num_args - entry->num_constant_args;
	zend_op_array *old_op_array = entry->orig_op_array;
	zend_op_array *new_op_array = zend_arena_alloc(&CG(arena), sizeof(zend_op_array));
	zend_op *new_opcodes = emalloc(sizeof(zend_op) * old_op_array->last);
	zval *new_literals = emalloc(sizeof(zval) * old_op_array->last_literal);
	zend_string **new_vars = emalloc(sizeof(zend_string *) * old_op_array->last_var);
	zend_live_range *new_live_range =
		emalloc(sizeof(zend_live_range) * old_op_array->last_live_range);
	zend_try_catch_element *new_try_catch_array =
		emalloc(sizeof(zend_try_catch_element) * old_op_array->last_try_catch);
	uint32_t *new_refcount = emalloc(sizeof(uint32_t));

	ALLOCA_FLAG(use_heap);
	uint32_t *cv_map = do_alloca(sizeof(uint32_t) * old_op_array->last_var, use_heap);

	uint32_t i, j;

	entry->cloned_op_array = new_op_array;

	/* Initialize CV map. The new order is:
	 * 1. The non-constant parameters
	 * 2. The constant paramters
	 * 3. All remaining variables */
	j = 0;
	for (i = 0; i < old_op_array->num_args; i++) {
		if (!Z_ISUNDEF(entry->args[i])) {
			continue;
		}
		cv_map[i] = j;
		j++;
	}
	for (i = 0; i < old_op_array->num_args; i++) {
		if (Z_ISUNDEF(entry->args[i])) {
			continue;
		}
		cv_map[i] = j;
		j++;
	}
	for (i = old_op_array->num_args; i < old_op_array->last_var; i++) {
		cv_map[i] = i;
	}

	memcpy(new_op_array, old_op_array, sizeof(zend_op_array));

	memcpy(new_live_range, old_op_array->live_range,
		sizeof(zend_live_range) * old_op_array->last_live_range);
	memcpy(new_try_catch_array, old_op_array->try_catch_array,
		sizeof(zend_try_catch_element) * old_op_array->last_try_catch);

	/* Copy literals */
	for (i = 0; i < old_op_array->last_literal; i++) {
		ZVAL_DUP(&new_literals[i], &old_op_array->literals[i]);
		new_literals[i].u2 = old_op_array->literals[i].u2;
	}

	/* Copy variable names. Take into account the new CV mapping */
	for (i = 0; i < old_op_array->last_var; i++) {
		new_vars[cv_map[i]] = zend_string_copy(old_op_array->vars[i]);
	}

	ZEND_ASSERT(new_op_array->function_name);
	//zend_string_addref(new_op_array->function_name);
	// TODO Why not?
	if (new_op_array->doc_comment) {
		zend_string_addref(new_op_array->doc_comment);
	}

	*new_refcount = 1;

	new_op_array->num_args = new_num_args;
	new_op_array->arg_info = clone_arg_info(entry, old_op_array);
	new_op_array->refcount = new_refcount;
	new_op_array->opcodes = new_opcodes;
	new_op_array->literals = new_literals;
	new_op_array->vars = new_vars;
	new_op_array->live_range = new_live_range;
	new_op_array->try_catch_array = new_try_catch_array;

	ZEND_ASSERT(new_op_array->run_time_cache == NULL);
	ZEND_ASSERT(new_op_array->static_variables == NULL);

	/* Literals must be assigned before this call */
	// TODO Directly insert the new literals into the literals table
	clone_opcodes(new_opcodes, entry, cv_map);
}

static void add_to_function_table(
		HashTable *function_table, cloned_func_entry *entry, uint32_t *counter) {
	zend_string *orig_name = entry->orig_op_array->function_name;
	entry->new_name = strpprintf(0, "%c%s%c%" PRIu32, '\0', ZSTR_VAL(orig_name), '\0', *counter);
	(*counter)++;

	if (!zend_hash_add_ptr(function_table, entry->new_name, entry->cloned_op_array)) {
		ZEND_ASSERT(0);
	}
	zend_string_release(entry->new_name);
}

static void replace_call(zend_call_info *call_info, cloned_func_entry *entry) {
	zend_op_array *op_array = call_info->caller_op_array;
	zend_op *init_opline = call_info->caller_init_opline;
	zval *func_name = RT_CONSTANT(op_array, init_opline->op2);
	uint32_t i, j;

	init_opline->extended_value = entry->cloned_op_array->num_args;

	/* At this point the literal is only referenced once, we can modify it directly */
	zend_string_release(Z_STR_P(func_name));
	ZVAL_STR_COPY(func_name, entry->new_name);

	j = 0;
	for (i = 0; i < call_info->num_args; i++) {
		zend_op *send_opline = call_info->arg_info[i].opline;
		if (Z_ISUNDEF(entry->args[i])) {
			send_opline->op2.num = j + 1;
			send_opline->result.num = NUM_VAR(j);
			j++;
		} else {
			ZEND_ASSERT(send_opline->opcode == ZEND_SEND_VAL);
			MAKE_NOP(send_opline);
		}
	}
	ZEND_ASSERT(j == entry->cloned_op_array->num_args);
}

void zend_optimize_cloning(zend_call_graph *call_graph, zend_optimizer_ctx *ctx) {
	uint32_t i;
	void *checkpoint = zend_arena_checkpoint(ctx->arena);
	cloned_func_entry *cloned_head = NULL, **cloned_current = &cloned_head;
	uint32_t counter = 0;

	for (i = 0; i < call_graph->op_arrays_count; i++) {
		zend_func_info *func_info = &call_graph->func_infos[i];
		zend_call_info *call_info;
		for (call_info = func_info->callee_info; call_info; call_info = call_info->next_callee) {
			cloned_func_entry *new_entry, *entry;
			uint32_t num_constant_args;

			if (!can_clone(call_info)) {
				continue;
			}

			num_constant_args = call_num_constant_args(call_info);
			if (!num_constant_args) {
				continue;
			}

			new_entry = create_cloned_func_entry(&ctx->arena, call_info, num_constant_args);
			entry = find_cloned_func_entry(cloned_head, new_entry);
			if (!entry) {
				entry = new_entry;
				clone_function(entry);
				add_to_function_table(&ctx->script->function_table, entry, &counter);
				OPT_STAT(cloned_funcs)++;

				/* Insert into cloned function list */
				*cloned_current = entry;
				cloned_current = &entry->next;
				entry = new_entry;
			}

			replace_call(call_info, entry);
		}
	}

	zend_arena_release(&ctx->arena, checkpoint);
}
