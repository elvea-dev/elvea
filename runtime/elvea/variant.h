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
 * Purpose: a polymorphic container for any elvea value.                                                               *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_VARIANT_H
#define ELVEA_VARIANT_H

#include <elvea/definitions.h>
#include <elvea/class.h>

#ifdef __cplusplus
extern "C" {
#endif

// Type of the payload of a variant.
typedef enum elvea_type_t
{
	ELVEA_TYPE_NULL    = 0, // this must always be 0
	ELVEA_TYPE_TRUE    = 1 << 0,
	ELVEA_TYPE_FALSE   = 1 << 1,
	ELVEA_TYPE_BOOLEAN = ELVEA_TYPE_TRUE|ELVEA_TYPE_FALSE,
	ELVEA_TYPE_NUMBER  = 1 << 2,
	ELVEA_TYPE_OBJECT  = 1 << 3,
	ELVEA_TYPE_ALIAS   = 1 << 4,
	ELVEA_TYPE_OPAQUE  = 1 == 5
} elvea_type_t;


//----------------------------------------------------------------------------------------------------------------------

// Polymorphic container for any elvea value. (This is implemented as a tagged union.)
struct elvea_variant_t
{
	union
	{
		elvea_float_t   number;
		elvea_size_t    integer; // for iterators only
		elvea_string_t *string;
		elvea_list_t   *list;
		elvea_table_t  *table;
		elvea_object_t *object;
		elvea_alias_t  *alias;
		elvea_class_t  *klass;
		void *any;

		struct {
			uint32_t x, y;
		} bits;
	} as;

	elvea_type_t type;
};


//----------------------------------------------------------------------------------------------------------------------

// An alias is a reference to a value.
struct elvea_alias_t
{
	// Alias metadata.
	struct elvea_metadata_t meta;

	// Value being referenced.
	elvea_variant_t variant;
};

//----------------------------------------------------------------------------------------------------------------------

elvea_alias_t *elvea_make_alias(elvea_thread_t *thread, elvea_variant_t *variant);

//----------------------------------------------------------------------------------------------------------------------

const char *elvea_get_class_name(elvea_thread_t *thread, const elvea_variant_t *variant);


// Type checking functions.

static inline
bool elvea_check_null(const elvea_variant_t *variant)
{
	return variant->type == ELVEA_TYPE_NULL;
}

static inline
bool elvea_check_bool(const elvea_variant_t *variant)
{
	return variant->type & ELVEA_TYPE_BOOLEAN;
}

static inline
bool elvea_check_num(const elvea_variant_t *variant)
{
	return variant->type == ELVEA_TYPE_NUMBER;
}

static inline
bool elvea_check_object(const elvea_variant_t *variant)
{
	return variant->type == ELVEA_TYPE_OBJECT;
}

static inline
bool elvea_check_alias(const elvea_variant_t *variant)
{
	return variant->type == ELVEA_TYPE_ALIAS;
}

static inline
void elvea_resolve_alias(elvea_thread_t *thread, elvea_variant_t **variant)
{
	while (elvea_check_alias(*variant)) {
		*variant = &(*variant)->as.alias->variant;
	}
}

bool elvea_check_string(elvea_thread_t *thread, elvea_variant_t *variant);

bool elvea_check_list(elvea_thread_t *thread, elvea_variant_t *variant);

bool elvea_check_table(elvea_thread_t *thread, elvea_variant_t *variant);


// Safely get a value of a given type. This functions throw if the type doesn't match.
// No implicit conversion is attempted.

bool elvea_get_bool(elvea_thread_t *thread, elvea_variant_t *variant);

elvea_float_t elvea_get_num(elvea_thread_t *thread, elvea_variant_t *variant);

elvea_string_t *elvea_get_string(elvea_thread_t *thread, elvea_variant_t *variant);

elvea_list_t *elvea_get_list(elvea_thread_t *thread, elvea_variant_t *variant);

elvea_table_t *elvea_get_table(elvea_thread_t *thread, elvea_variant_t *variant);

elvea_string_t **elvea_get_string_ref(elvea_thread_t *thread, elvea_variant_t *variant);

elvea_list_t **elvea_get_list_ref(elvea_thread_t *thread, elvea_variant_t *variant);

elvea_table_t **elvea_get_table_ref(elvea_thread_t *thread, elvea_variant_t *variant);


//----------------------------------------------------------------------------------------------------------------------

static inline
void elvea_zero(elvea_variant_t *variant)
{
	variant->type = ELVEA_TYPE_NULL;
}

void elvea_retain(elvea_thread_t *thread, elvea_variant_t *variant);

void elvea_release(elvea_thread_t *thread, elvea_variant_t *variant);

void elvea_copy(elvea_thread_t *thread, elvea_variant_t *dst, const elvea_variant_t *src);

void elvea_move(elvea_thread_t *thread, elvea_variant_t *dst, elvea_variant_t *src);

void elvea_swap(elvea_thread_t *thread, elvea_variant_t *variant1, elvea_variant_t *variant2);

bool elvea_equal(elvea_thread_t *thread, elvea_variant_t *self, elvea_variant_t *other);

int elvea_compare(elvea_thread_t *thread, elvea_variant_t *self, elvea_variant_t *other);

elvea_size_t elvea_hash(elvea_thread_t *thread, elvea_variant_t *variant);

void elvea_clear(elvea_thread_t *thread, elvea_variant_t *variant);

#ifdef __cplusplus
}
#endif

#endif // ELVEA_VARIANT_H
