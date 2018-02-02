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
 * Purpose: common definitions.                                                                                        *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_DEFINITIONS_H
#define ELVEA_DEFINITIONS_H

#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Determine architecture
static const bool ELVEA_ARCH64 = (sizeof(void *) == 8);
static const bool ELVEA_ARCH32 = (sizeof(void *) == 4);


//----------------------------------------------------------------------------------------------------------------------

// All elvea numbers are represented internally as double precision floating point number.
typedef double elvea_float_t;

// Type for sizes and and indices. Indices are expressed in base 1 and can be negative.
typedef intptr_t elvea_index_t;

// Size type to represent the number of elements in a collection.
typedef uint32_t elvea_size_t;

// Invalid size/index (non-position).
#define ELVEA_NPOS UINT32_MAX

// Format size.
#define ELVEA_FORMAT_SIZE PRIu32

// Format index.
#define ELVEA_FORMAT_INDEX PRIdPTR


//----------------------------------------------------------------------------------------------------------------------

// Color for the garbage collector. Objects that are acyclic (i.e. contain no cyclic reference)
// are green. Base types such as string and regex are acyclic because there is no way they
// can store a reference to themselves. Collections (list, table, etc.) are considered cyclic
// and are therefore candidates for GC.
typedef enum elvea_gc_color_t
{
	ELVEA_GC_GREEN,      // object which is not collectable
	ELVEA_GC_BLACK,      // Assumed to be alive
	ELVEA_GC_GREY,       // Possible member of a GC cycle
	ELVEA_GC_WHITE,      // Possibly dead
	ELVEA_GC_PURPLE      // Root candidate for a GC cycle
} elvea_gc_color_t;

// Forward declarations.
typedef struct elvea_variant_t elvea_variant_t;
typedef struct elvea_class_t elvea_class_t;
typedef struct elvea_object_t elvea_object_t;
typedef struct elvea_string_t elvea_string_t;
typedef struct elvea_list_t elvea_list_t;
typedef struct elvea_table_t elvea_table_t;
typedef struct elvea_alias_t elvea_alias_t;
typedef struct elvea_thread_t elvea_thread_t;
typedef struct elvea_iterator_t elvea_iterator_t;
typedef struct elvea_recycler_t elvea_recycler_t;
typedef struct elvea_runtime_t elvea_runtime_t;

// Memory allocator. It must be similar to realloc but free the memory block if [new_size] is 0.
typedef void*(*elvea_allocator_t)(void* ptr, size_t old_size, size_t new_size);

// Custom error handler. When an error is thrown, the error code and error message are passed to
// this handler, if it exists, otherwise it uses the default handler.
typedef void(*elvea_error_callback_t)(int code, const char *message);

// Finalize resources owned by the object. This method must NOT release the object itself,
// because this is done automatically after the finalizer has been called.
typedef void(*elvea_finalize_callback_t)(elvea_thread_t*, elvea_object_t*);

// Perform a deep copy of an object.
typedef elvea_object_t*(*elvea_clone_callback_t)(elvea_thread_t*, const elvea_object_t*);

// Traverse the members of an object. This is used by the GC to detect and break reference cycles. The void* is a
// context (which may be NULL) passed along the object to the callback function; the return value can be used to
// signal something to the caller, but it should be ELVEA_RESULT_SUCCESS in most cases.
typedef int(*elvea_traverse_callback_t)(elvea_thread_t*, elvea_object_t*, void*);

// Compare 2 objects of the same type. It must return a negative value if a < b, 0 if a == b
// and a positive value if a > b.
typedef int(*elvea_compare_callback_t)(elvea_thread_t*, const elvea_object_t*, const elvea_object_t*);

// Compare 2 objects of the same type for equality. If this method is not implemented but `compare` is,
// `compare` will be used instead.
typedef bool(*elvea_equal_callback_t)(elvea_thread_t*, const elvea_object_t*, const elvea_object_t*);

// Produce a hash value for an object, so that it can be stored in a table. Two objects that compare equal must
// hash to the same value.
typedef uint32_t(*elvea_hash_callback_t)(elvea_thread_t*, elvea_object_t*);

// Return the next item in an iterable object. Returns true if an item was found, and false if reached the end of the
// collection.
typedef bool(*elvea_iterate_callback_t)(elvea_thread_t*, elvea_object_t *, elvea_variant_t *state, elvea_variant_t *result);


//----------------------------------------------------------------------------------------------------------------------

// Header for all reference-counted structures (objects and aliases).
struct elvea_metadata_t
{
	// Number of strong references to the object.
	uint32_t ref_count;

	// Flag for the GC.
	elvea_gc_color_t gc_color : 3;

	// Flag for aliases and variants allocated from the memory arena.
	bool arena : 1;

	// Reserved for future use.
	uint16_t reserved : 12;

	// Flags for subclasses to do all sorts of naughty things...
	uint16_t flags;
};

// Base class for all non-primitive objects.
struct elvea_object_t
{
	// Object metadata.
	struct elvea_metadata_t meta;

	// Type of the object.
	elvea_class_t *isa;
};


//----------------------------------------------------------------------------------------------------------------------

// Convenience macros.

#define ELVEA_MIN(x, y) (x) < (y) ? (x) : (y)
#define ELVEA_MAX(x, y) (x) > (y) ? (x) : (y)

#define ELVEA_CLONABLE true
#define ELVEA_NONCLONABLE false

#define elvea_ref(obj) ++(((struct elvea_metadata_t*)(obj))->ref_count)
#define elvea_unref(obj) (--(((struct elvea_metadata_t*)(obj))->ref_count) == 0)

#define ELVEA_UNUSED(x) (void)x


#define elvea_is_shared(obj) (((struct elvea_metadata_t*)(obj))->ref_count > 1)

#define elvea_assign(thread, dst, src) elvea_object_release(thread, dst); (dst = src); elvea_object_retain(thread, dst)

//----------------------------------------------------------------------------------------------------------------------

// Create a new instance of a given class.
void *elvea_new(elvea_thread_t *thread, elvea_class_t *type, bool collectable, int extra);

// Destroy an object.
void elvea_delete(elvea_thread_t *thread, void *ptr);

// Non-collectable are always colored green and are not tracked by the GC.
static inline
bool elvea_is_collectable(const elvea_object_t *self)
{
	return self->meta.gc_color != ELVEA_GC_GREEN;
}

static inline
void elvea_object_retain(elvea_thread_t *thread, void *ptr)
{
	elvea_ref(ptr);
}

static inline
void elvea_alias_retain(elvea_thread_t *thread, void *ptr)
{
	elvea_ref(ptr);
}

void elvea_object_release(elvea_thread_t *thread, void *ptr);

void elvea_alias_release(elvea_thread_t *thread, elvea_alias_t *alias);

// Checks that a memory block allocated by the allocator is not NULL, and throw an error if it is.
bool elvea_check_memory(elvea_thread_t *thread, void *ptr);


//----------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif


// Configuration options.
#include "config.h"

#endif // ELVEA_DEFINITIONS_H
