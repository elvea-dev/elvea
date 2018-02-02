/***********************************************************************************************************************
 *                                                                                                                     *
 * Copyright (C) 2017-2018 Julien Eychenne                                                                             *
 *                                                                                                                     *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated        *
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the *
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to     *
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:                     *
 *                                                                                                                     *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of    *
 * the Software.                                                                                                       *
 *                                                                                                                     *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO    *
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, *
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE      *
 * SOFTWARE.                                                                                                           *
 *                                                                                                                     *
 * Created: 2018.02.01                                                                                                 *
 *                                                                                                                     *
 * Purpose: see header.                                                                                                *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#include <float.h>
#include <math.h>
#include <elvea/variant.h>
#include <elvea/error.h>
#include <elvea/thread.h>
#include "variant.h"

static
bool elvea_num_equal(double x, double y)
{
	// Comparison method by Christer Ericson. See http://realtimecollisiondetection.net/blog/?p=89
	double scale = ELVEA_MAX(1.0, ELVEA_MAX(fabs(x), fabs(y)));
	return fabs(x-y) <= DBL_EPSILON * scale;
}

const char *elvea_get_class_name(elvea_thread_t *thread, const elvea_variant_t *variant)
{
	switch (variant->type)
	{
		case ELVEA_TYPE_NULL:
			return "null";
		case ELVEA_TYPE_TRUE:
		case ELVEA_TYPE_FALSE:
			return "bool";
		case ELVEA_TYPE_NUMBER:
			return "num";
		case ELVEA_TYPE_OBJECT:
			return variant->as.object->isa->name;
		case ELVEA_TYPE_ALIAS:
		{
			return elvea_get_class_name(thread, &variant->as.alias->variant);
		}
		default:
			return "unknown";
	}
}

bool elvea_check_string(elvea_thread_t *thread, elvea_variant_t *variant)
{
	elvea_resolve_alias(thread, &variant);
	return elvea_check_object(variant) && variant->as.object->isa == thread->string_class;
}

bool elvea_check_list(elvea_thread_t *thread, elvea_variant_t *variant)
{
	elvea_resolve_alias(thread, &variant);
	return elvea_check_object(variant) && variant->as.object->isa == thread->list_class;
}

bool elvea_check_table(elvea_thread_t *thread, elvea_variant_t *variant)
{
	elvea_resolve_alias(thread, &variant);
	return elvea_check_object(variant) && variant->as.object->isa == thread->table_class;
}

bool elvea_get_bool(elvea_thread_t *thread, elvea_variant_t *variant)
{
	if (!elvea_check_bool(variant)) {
		elvea_throw(thread, ELVEA_ERROR_TYPE, "expected a boolean, not a %s", elvea_get_class_name(thread, variant));
	}

	return variant->type == ELVEA_TYPE_TRUE;
}

elvea_float_t elvea_get_num(elvea_thread_t *thread, elvea_variant_t *variant)
{
	if (!elvea_check_num(variant)) {
		elvea_throw(thread, ELVEA_ERROR_TYPE, "expected a number, not a %s", elvea_get_class_name(thread, variant));
	}

	return variant->type == ELVEA_TYPE_TRUE;
}

elvea_string_t *elvea_get_string(elvea_thread_t *thread, elvea_variant_t *variant)
{
	if (!elvea_check_string(thread, variant)) {
		elvea_throw(thread, ELVEA_ERROR_TYPE, "expected a string, not a %s", elvea_get_class_name(thread, variant));
	}

	return variant->as.string;
}

elvea_list_t *elvea_get_list(elvea_thread_t *thread, elvea_variant_t *variant)
{
	if (!elvea_check_list(thread, variant)) {
		elvea_throw(thread, ELVEA_ERROR_TYPE, "expected a list, not a %s", elvea_get_class_name(thread, variant));
	}

	return variant->as.list;
}

elvea_table_t *elvea_get_table(elvea_thread_t *thread, elvea_variant_t *variant)
{
	if (!elvea_check_table(thread, variant)) {
		elvea_throw(thread, ELVEA_ERROR_TYPE, "expected a table, not a %s", elvea_get_class_name(thread, variant));
	}

	return variant->as.table;
}

elvea_string_t **elvea_get_string_ref(elvea_thread_t *thread, elvea_variant_t *variant)
{
	if (!elvea_check_string(thread, variant)) {
		elvea_throw(thread, ELVEA_ERROR_TYPE, "expected a string, not a %s", elvea_get_class_name(thread, variant));
	}

	return &variant->as.string;
}

elvea_list_t **elvea_get_list_ref(elvea_thread_t *thread, elvea_variant_t *variant)
{
	if (!elvea_check_list(thread, variant)) {
		elvea_throw(thread, ELVEA_ERROR_TYPE, "expected a list, not a %s", elvea_get_class_name(thread, variant));
	}

	return &variant->as.list;
}

elvea_table_t **elvea_get_table_ref(elvea_thread_t *thread, elvea_variant_t *variant)
{
	if (!elvea_check_table(thread, variant)) {
		elvea_throw(thread, ELVEA_ERROR_TYPE, "expected a table, not a %s", elvea_get_class_name(thread, variant));
	}

	return &variant->as.table;
}


//----------------------------------------------------------------------------------------------------------------------

elvea_alias_t *elvea_make_alias(elvea_thread_t *thread, elvea_variant_t *variant)
{
	if (elvea_check_alias(variant)) {
		return variant->as.alias;
	}

	// TODO: initialize alias!!
	elvea_alias_t *alias = elvea_alloc_alias(thread);
}

//----------------------------------------------------------------------------------------------------------------------

static inline
void raw_copy(elvea_variant_t *dst, const elvea_variant_t *src)
{
	dst->type = src->type;
	dst->as = src->as;
}

void elvea_retain(elvea_thread_t *thread, elvea_variant_t *variant)
{
	if (elvea_check_object(variant)) {
		elvea_object_retain(thread, variant->as.object);
	}
	else if (elvea_check_alias(variant)) {
		elvea_alias_retain(thread, variant->as.alias);
	}
}

void elvea_release(elvea_thread_t *thread, elvea_variant_t *variant)
{
	if (elvea_check_object(variant)) {
		elvea_object_release(thread, variant->as.object);
	}
	else if (elvea_check_alias(variant)) {
		elvea_alias_release(thread, variant->as.alias);
	}
}

void elvea_copy(elvea_thread_t *thread, elvea_variant_t *dst, const elvea_variant_t *src)
{
	elvea_release(thread, dst);
	raw_copy(dst, src);
	elvea_retain(thread, dst);
}

void elvea_move(elvea_thread_t *thread, elvea_variant_t *dst, elvea_variant_t *src)
{
	elvea_release(thread, dst);
	raw_copy(dst, src);
	elvea_zero(dst);
}

void elvea_clear(elvea_thread_t *thread, elvea_variant_t *variant)
{
	elvea_release(thread, variant);
	elvea_zero(variant);
}

void elvea_swap(elvea_thread_t *thread, elvea_variant_t *variant1, elvea_variant_t *variant2)
{
	elvea_variant_t tmp;

	raw_copy(&tmp, variant1);
	raw_copy(variant1, variant2);
	raw_copy(variant2, &tmp);
}

bool elvea_equal(elvea_thread_t *thread, elvea_variant_t *self, elvea_variant_t *other)
{
	elvea_resolve_alias(thread, &self);
	elvea_resolve_alias(thread, &other);

	if (self->type != other->type)
	{
		if (elvea_check_bool(self) && elvea_check_bool(other))
		{
			return self->type == other->type;
		}
		else if (elvea_check_num(self) && elvea_check_num(other))
		{
			elvea_float_t n1 = self->as.number;
			elvea_float_t n2 = other->as.number;

			return elvea_num_equal(n1, n2);
		}

		// Different types, can't be equal.
		return false;
	}

	switch (self->type)
	{
		case ELVEA_TYPE_NULL:
		case ELVEA_TYPE_TRUE:
		case ELVEA_TYPE_FALSE:
			return true;

		case ELVEA_TYPE_NUMBER:
			return elvea_num_equal(self->as.number, other->as.number);

		case ELVEA_TYPE_OBJECT:
		{
			elvea_equal_callback_t equal = self->as.object->isa->equal;

			if (equal) {
				return equal(thread, self->as.object, other->as.object);
			}

			// If the type doesn't implement equals$(), try to use compare$().
			elvea_compare_callback_t cmp = self->as.object->isa->compare;

			if (cmp) {
				return cmp(thread, self->as.object, other->as.object) == 0;
			}

			elvea_throw(thread, ELVEA_ERROR_RUNTIME, "Type %s does not support comparison for equality.\n"
								"Hint: if you need comparison, implement equal$() or compare$() for this type.",
						elvea_get_class_name(thread, self));
			break;
		}

		default:
			elvea_throw(thread, ELVEA_ERROR_RUNTIME, "Cannot compare values of type %s",
						elvea_get_class_name(thread, self));
	}

	return false;
}

int elvea_compare(elvea_thread_t *thread, elvea_variant_t *self, elvea_variant_t *other)
{
	elvea_resolve_alias(thread, &self);
	elvea_resolve_alias(thread, &other);

	if (self->type != other->type)
	{
		if (elvea_check_bool(self) && elvea_check_bool(other))
		{
			int b1 = self->type & ELVEA_TYPE_TRUE;
			int b2 = other->type & ELVEA_TYPE_TRUE;

			return b1 - b2;
		}
		else if (elvea_check_num(self) && elvea_check_num(other))
		{
			elvea_float_t n1 = self->as.number;
			elvea_float_t n2 = other->as.number;

			return (int) (n1 - n2);
		}

		elvea_throw(thread, ELVEA_ERROR_TYPE, "Cannot compare values of type %s and %s",
					elvea_get_class_name(thread, self), elvea_get_class_name(thread, other));
	}

	switch (self->type)
	{
		case ELVEA_TYPE_NULL:
		case ELVEA_TYPE_TRUE:
		case ELVEA_TYPE_FALSE:
			return 0;

		case ELVEA_TYPE_NUMBER:
		{
			elvea_float_t n1 = self->as.number;
			elvea_float_t n2 = other->as.number;

			return elvea_num_equal(n1, n2) ? 0 : ((n1 - n2) < 0 ? -1 : 1);
		}
		case ELVEA_TYPE_OBJECT:
		{
			elvea_compare_callback_t cmp = self->as.object->isa->compare;

			if (cmp) {
				return cmp(thread, self->as.object, other->as.object);
			}

			elvea_throw(thread, ELVEA_ERROR_RUNTIME, "Type %s does not support comparison.\n"
								"Hint: if you need comparison, implement compare$() for this type.",
						elvea_get_class_name(thread, self));
			break;
		}

		default:
			elvea_throw(thread, ELVEA_ERROR_RUNTIME, "Cannot compare values of type %s",
						elvea_get_class_name(thread, self));
	}

	return INT32_MIN; // No useful value to return.
}

elvea_size_t elvea_hash(elvea_thread_t *thread, elvea_variant_t *variant)
{
	elvea_resolve_alias(thread, &variant);

	switch (variant->type)
	{
		case ELVEA_TYPE_TRUE:
			return 7;

		case ELVEA_TYPE_FALSE:
			return 11;

		case ELVEA_TYPE_NUMBER:
			return variant->as.bits.x + variant->as.bits.y;

		case ELVEA_TYPE_OBJECT:
		{
			elvea_hash_callback_t hash = variant->as.object->isa->hash;

			if (hash) {
				return hash(thread, variant->as.object);
			}
		}
		default:
			elvea_throw(thread, ELVEA_ERROR_RUNTIME, "Type %s is not hashable", elvea_get_class_name(thread, variant));
	}
}
void elvea_init_bool(elvea_thread_t *thread, elvea_variant_t *variant, bool b);
void elvea_init_num(elvea_thread_t *thread, elvea_variant_t *variant, double n);
void elvea_init_object(elvea_thread_t *thread, elvea_variant_t *variant, void *object);
void elvea_set_bool(elvea_thread_t *thread, elvea_variant_t *variant, bool b);
void elvea_set_num(elvea_thread_t *thread, elvea_variant_t *value, double n);
void elvea_set_object(elvea_thread_t *thread, elvea_variant_t *value, void *object);

void elvea_init_bool(elvea_thread_t *thread, elvea_variant_t *variant, bool b)
{
	variant->type = b ? ELVEA_TYPE_TRUE : ELVEA_TYPE_FALSE;
}

void elvea_init_num(elvea_thread_t *thread, elvea_variant_t *variant, double n)
{
	variant->type = ELVEA_TYPE_NUMBER;
	variant->as.number = n;
}

void elvea_init_object(elvea_thread_t *thread, elvea_variant_t *variant, void *object)
{
	variant->type = ELVEA_TYPE_OBJECT;
	variant->as.any = object;
	elvea_retain(thread, variant->as.any);
}

void elvea_set_bool(elvea_thread_t *thread, elvea_variant_t *variant, bool b)
{
	elvea_release(thread, variant);
	elvea_init_bool(thread, variant, b);
}

void elvea_set_num(elvea_thread_t *thread, elvea_variant_t *value, double n)
{
	elvea_release(thread, value);
	elvea_init_num(thread, value, n);
}


void elvea_set_object(elvea_thread_t *thread, elvea_variant_t *value, void *object)
{
	elvea_release(thread, value);
	elvea_init_object(thread, value, object);
}
