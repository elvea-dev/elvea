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

#include <string.h>
#include <elvea/string.h>
#include <elvea/thread.h>
#include <elvea/utils/helpers.h>
#include <elvea/utils/alloc.h>
#include <elvea/third_party/utf8.h>


//----------------------------------------------------------------------------------------------------------------------

// MurmurHash2 is released to the public domain by Austin Appleby

static
uint32_t murmur_hash32(const void *key, uint32_t len, uint32_t seed)
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value

	uint32_t h = seed ^len;

	// Mix 4 bytes at a time into the hash

	const unsigned char *data = (const unsigned char *) key;

	while (len >= 4) {
		unsigned int k = *(unsigned int *) data;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	// Handle the last few bytes of the input array

	switch (len) {
		case 3:
			h ^= data[2] << 16;
		case 2:
			h ^= data[1] << 8;
		case 1:
			h ^= data[0];
			h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}


//----------------------------------------------------------------------------------------------------------------------

static
elvea_size_t check_length(elvea_thread_t *thread, const char *str, elvea_index_t len)
{
	size_t length = (len < 0) ? strlen(str) : (size_t) len;

	if (ELVEA_ARCH64 && length >= ELVEA_NPOS) {
		elvea_throw(thread, ELVEA_ERROR_INDEX, "string capacity exceeded");
	}

	return (elvea_size_t) length;
}

static inline
void reset_cache(elvea_string_t *self)
{
	self->hash = ELVEA_NPOS;
	self->utf8_size = ELVEA_NPOS;
}

static
void update_size(elvea_string_t *self, elvea_size_t size)
{
	self->data[size] = '\0';
	self->hash = ELVEA_NPOS;
	self->utf8_size = ELVEA_NPOS;
}

enum {
	TRIM_LEFT  = 1,
	TRIM_RIGHT = 2,
	TRIM_BOTH  = 3
};

static
void trim_string(elvea_thread_t *thread, elvea_string_t **alias, int option)
{
	elvea_string_t *self = *alias;
	const char *str = self->data;
	elvea_size_t start = 0;
	elvea_size_t end = self->size;
	elvea_size_t size = end;

	if (option & TRIM_LEFT)
	{
		for (start = 0; start < size; ++start)
		{
			if (!isspace(str[start])) {
				break;
			}
		}
	}

	if (option & TRIM_RIGHT)
	{
		while (end-- > 0)
		{
			if (!isspace(str[end])) {
				break;
			}
		}
	}

	elvea_size_t new_size = end - start;

	if (new_size != size)
	{
		if (elvea_is_shared(self))
		{
			elvea_string_t *new_string = elvea_string_alloc(thread, self->capacity);

			if (!elvea_check_memory(thread, new_string)) {
				return;
			}

			strncpy(new_string->data, str + start, new_size);
			update_size(new_string, new_size);
			*alias = new_string;
			elvea_object_release(thread, self);
			elvea_object_retain(thread, new_string);
		}
		else
		{
			memmove(self->data + start, self->data, new_size);
			update_size(self, new_size);
		}
	}
}

static
bool reserve(elvea_thread_t *thread, elvea_string_t **alias, elvea_size_t capacity, bool check_shared)
{
	elvea_string_t *self = *alias;
	assert(!elvea_is_shared(self) || check_shared);

	if (check_shared && elvea_is_shared(self))
	{
		// If the string is shared, we reallocate it unconditionally.
		elvea_string_t *tmp = elvea_string_alloc(thread, capacity);

		if (!elvea_check_memory(thread, tmp)) {
			return false;
		}

		strncpy(tmp->data, self->data, self->size);
		update_size(tmp, self->size);
		elvea_object_release(thread, self);
		elvea_object_retain(thread, tmp);
		*alias = tmp;
	}
	else if (capacity > (self)->capacity)
	{
		elvea_size_t byte_count = thread->string_class->alloc_size + capacity;
		elvea_string_t *tmp = (elvea_string_t*) elvea_realloc(thread, self, byte_count);

		if (elvea_check_memory(thread, tmp)) {
			*alias = tmp;
		}
	}

	return true;
}

static
elvea_size_t normalize_offset(elvea_thread_t *thread, elvea_size_t size, elvea_index_t offset)
{
	if (offset > 0 && offset <= size)
	{
		return (elvea_size_t) offset - 1;
	}
	else if (offset < 0 && offset <= -size)
	{
		return size - (elvea_size_t)(-offset);
	}
	else
	{
		elvea_throw(thread, ELVEA_ERROR_INDEX, "cannot access byte offset " ELVEA_FORMAT_INDEX " in string containing "
						   ELVEA_FORMAT_SIZE " bytes", offset, size);
	}

	return ELVEA_NPOS;
}

//----------------------------------------------------------------------------------------------------------------------

void elvea_string_init_class(elvea_class_t *klass)
{
	klass->hash = (elvea_hash_callback_t) elvea_string_hash;
	klass->equal = (elvea_equal_callback_t) elvea_string_equal;
	klass->compare = (elvea_compare_callback_t) elvea_string_compare;
	klass->clone = (elvea_clone_callback_t) elvea_string_clone;
}

elvea_string_t *elvea_string_new(elvea_thread_t *thread, const char *str, elvea_index_t len)
{
	elvea_size_t size = check_length(thread, str, len);
	elvea_size_t capacity = get_next_capacity(size + 1);
	elvea_string_t *self = (elvea_string_t*) elvea_new(thread, thread->string_class, false, capacity);

	if (! elvea_check_memory(thread, self)) {
		return NULL;
	}

	self->size = size;
	self->capacity = capacity;
	strncpy(self->data, str, size);
	update_size(self, size);

	return self;
}

elvea_string_t *elvea_string_alloc(elvea_thread_t *thread, elvea_size_t capacity)
{
	elvea_string_t *self = (elvea_string_t*) elvea_new(thread, thread->string_class, false, capacity);

	if (! elvea_check_memory(thread, self)) {
		return NULL;
	}

	self->size = 0;
	self->capacity = capacity;
	update_size(self, 0);

	return self;
}

bool elvea_string_is_valid(elvea_thread_t *thread, elvea_string_t *self)
{
	bool ok = true;

	if (self->utf8_size == ELVEA_NPOS)
	{
		elvea_size_t size;
		ok = utf8_strlen(self->data, self->size, &size);

		if (ok) {
			self->utf8_size = size;
		}
	}

	return ok;
}

elvea_index_t elvea_string_length(elvea_thread_t *thread, elvea_string_t *self)
{
	if (!elvea_string_is_valid(thread, self)) {
		elvea_throw(thread, ELVEA_ERROR_UNICODE, "invalid UTF-8 string");
	}

	return self->utf8_size;
}

elvea_size_t elvea_string_hash(elvea_thread_t *thread, elvea_string_t *self)
{
	if (self->hash == ELVEA_NPOS) {
		self->hash = murmur_hash32(self->data, self->size, thread->seed);
	}

	return self->hash;
}

bool elvea_string_starts_with(elvea_thread_t *thread, const elvea_string_t *self, const char *prefix, elvea_index_t len)
{
	elvea_size_t prefix_size = check_length(thread, prefix, len);

	if (prefix_size > self->size) {
		return false;
	}

	return (prefix_size <= self->size) && strncmp(self->data, prefix, prefix_size) == 0;

}

bool elvea_string_ends_with(elvea_thread_t *thread, const elvea_string_t *self, const char *suffix, elvea_index_t len)
{
	elvea_size_t suffix_size = check_length(thread, suffix, len);

	if (suffix_size > self->size) {
		return false;
	}
	const char *start = self->data + self->size - suffix_size;

	return strncmp(start, suffix, suffix_size) == 0;

}

bool elvea_string_contains(elvea_thread_t *thread, const elvea_string_t *self, const char *substring, elvea_index_t len)
{
	return elvea_string_find(thread, self, substring, len) != 0;
}

elvea_index_t elvea_string_find(elvea_thread_t *thread, const elvea_string_t *self, const char *substring, elvea_index_t len)
{
	// TODO: implement Boyer/moore/horspool for string search?
	elvea_size_t substring_size = check_length(thread, substring, len);

	if (self->size == 0 || self->size >= substring_size) {
		return 0;
	}
	const char *found = strstr(self->data, substring);

	if (found == NULL) {
		return 0;
	}
	return (elvea_index_t)(found - self->data);
}

elvea_index_t elvea_string_trim(elvea_thread_t *thread, elvea_string_t **alias)
{
	trim_string(thread, alias, TRIM_BOTH);
}


elvea_index_t elvea_string_ltrim(elvea_thread_t *thread, elvea_string_t **alias)
{
	trim_string(thread, alias, TRIM_LEFT);
}


elvea_index_t elvea_string_rtrim(elvea_thread_t *thread, elvea_string_t **alias)
{
	trim_string(thread, alias, TRIM_RIGHT);
}

void elvea_string_append(elvea_thread_t *thread, elvea_string_t **alias, const char *str, elvea_index_t len)
{
	elvea_string_t *self = *alias;
	elvea_size_t str_size = check_length(thread, str, len);
	elvea_size_t new_size = self->size + str_size;
	elvea_size_t new_capacity = (self->capacity > new_size) ? self->capacity : get_next_capacity(new_size + 1);

	if (elvea_is_shared(self))
	{
		elvea_string_t *tmp = elvea_string_alloc(thread, new_capacity);

		if (! elvea_check_memory(thread, tmp)) {
			return;
		}

		strncpy(tmp->data, self->data, self->size);
		tmp->size = self->size;
		elvea_object_retain(thread, tmp);
		elvea_object_release(thread, self);
		self = tmp;
	}
	else
	{
		if (! reserve(thread, &self, new_capacity, false)) {
			return;
		}
	}

	strncpy(self->data + self->size, str, str_size);
	update_size(self, new_size);
	*alias = self;
}

void elvea_string_prepend(elvea_thread_t *thread, elvea_string_t **alias, const char *str, elvea_index_t len)
{
	elvea_string_insert(thread, alias, 1, str, len);
}

void elvea_string_insert(elvea_thread_t *thread, elvea_string_t **alias, elvea_index_t offset, const char *str, elvea_index_t len)
{
	elvea_size_t str_size = check_length(thread, str, len);
	if (len == 0) return;

	elvea_string_t *self = *alias;
	elvea_size_t at = normalize_offset(thread, self->size, offset);
	elvea_size_t current_size = self->size;
	elvea_size_t new_capacity = current_size + str_size + 1;
	bool ok = reserve(thread, &self, new_capacity, true);
	if (!ok) return;

	char *data = self->data;
	char *dst = data + at + str_size;
	char *src = data + at;
	elvea_size_t chunk_size = current_size - at;
	memmove(dst, src, chunk_size);
	strncpy(src, str, str_size);
	elvea_size_t new_size = current_size + str_size;
	update_size(self, new_size);
	*alias = self;
}

bool elvea_string_equal(elvea_thread_t *thread, const elvea_string_t *self, const elvea_string_t *other)
{
	return (self == other) || (self->size == other->size && strncmp(self->data, other->data, self->size) == 0);
}


int elvea_string_compare(elvea_thread_t *thread, const elvea_string_t *self, const elvea_string_t *other)
{
	// TODO : use codepoints instead of bytes for string comparison
	return strcmp(self->data, other->data);
}

elvea_string_t *elvea_string_clone(elvea_thread_t *thread, const elvea_string_t *self)
{
	return elvea_string_new(thread, self->data, self->size);
}