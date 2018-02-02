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
 * Purpose: a dynamic, nul-terminated UTF-8 string. Indexes are in base 1 and can be negative. They represent byte     *
 * offsets.                                                                                                            *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_STRING_H
#define ELVEA_STRING_H

#include <elvea/definitions.h>

#ifdef __cplusplus
extern "C" {
#endif

struct elvea_string_t
{
	// Common base.
	elvea_object_t base;

	// Number of bytes in the string, excluding the nul terminator.
	elvea_size_t size;

	// Capacity of the char array, including the nul terminator. (capacity > size).
	elvea_size_t capacity;

	// Cached hash value.
	elvea_size_t hash;

	// Cached UTF-8 size.
	elvea_size_t utf8_size;

	// Beginning of the char data (more is allocated after that).
	char data[1];
};


//----------------------------------------------------------------------------------------------------------------------

void elvea_string_init_class(elvea_class_t *klass);

// Create a string object from a C string. If [len] is negative, the string's length is computed with strlen().
elvea_string_t *elvea_string_new(elvea_thread_t *thread, const char *str, elvea_index_t len);

// Allocate an empty string with a given capacity. (For internal use only.)
elvea_string_t *elvea_string_alloc(elvea_thread_t *thread, elvea_size_t capacity);

// Check that the string is valid UTF-8. This computes the string's length as a side effect.
bool elvea_string_is_valid(elvea_thread_t *thread, elvea_string_t *self);

// Get the number of Unicode code points in the string. Throw an error is the string is invalid.
elvea_index_t elvea_string_length(elvea_thread_t *thread, elvea_string_t *self);

// Hash the content of the string.
elvea_size_t elvea_string_hash(elvea_thread_t *thread, elvea_string_t *self);

// Check whether a string starts with a given prefix.
bool elvea_string_starts_with(elvea_thread_t *thread, const elvea_string_t *self, const char *prefix, elvea_index_t len);

// Check whether a string ends with a given suffix.
bool elvea_string_ends_with(elvea_thread_t *thread, const elvea_string_t *self, const char *suffix, elvea_index_t len);

// Check whether a string contains a given substring.
bool elvea_string_contains(elvea_thread_t *thread, const elvea_string_t *self, const char *substring, elvea_index_t len);

// Return the byte offset (in base 1) of the beginning of the first instance of the substring, or 0 if it wasn't found.
elvea_index_t elvea_string_find(elvea_thread_t *thread, const elvea_string_t *self, const char *substring, elvea_index_t len);

// Trim blank characters at both ends of the string.
elvea_index_t elvea_string_trim(elvea_thread_t *thread, elvea_string_t **alias);

// Trim blank characters at the beginning.
elvea_index_t elvea_string_ltrim(elvea_thread_t *thread, elvea_string_t **alias);

// Trim blank characters at the end.
elvea_index_t elvea_string_rtrim(elvea_thread_t *thread, elvea_string_t **alias);

// Append a string at the end of another string.
void elvea_string_append(elvea_thread_t *thread, elvea_string_t **alias, const char *str, elvea_index_t len);

// Prepend a string at the beginning of another string.
void elvea_string_prepend(elvea_thread_t *thread, elvea_string_t **alias, const char *str, elvea_index_t len);

// Insert string at the given byte offset (in base 1).
void elvea_string_insert(elvea_thread_t *thread, elvea_string_t **alias, elvea_index_t offset, const char *str, elvea_index_t len);

// Compare two strings for equality.
bool elvea_string_equal(elvea_thread_t *thread, const elvea_string_t *self, const elvea_string_t *other);

// Compare two strings lexicographically.
int elvea_string_compare(elvea_thread_t *thread, const elvea_string_t *self, const elvea_string_t *other);

// Create a deep copy of a string.
elvea_string_t *elvea_string_clone(elvea_thread_t *thread, const elvea_string_t *self);

#ifdef __cplusplus
}
#endif

#endif // ELVEA_STRING_H
