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
 * Created: 2018.01.13                                                                                                 *
 *                                                                                                                     *
 * Purpose: see header.                                                                                                *
 *                                                                                                                     *
 * Note: This data type is based on an open source implementation, which came with the following copyright notice:     *
 *                                                                                                                     *
 * =================================================================================================================== *
 * Copyright (C) 2007 The Android Open Source Project                                                                  *
 *                                                                                                                     *
 * Licensed under the Apache License, Version 2.0 (the "License") you may not use this file except in compliance with  *
 * the License. You may obtain a copy of the License at                                                                *
 *                                                                                                                     *
 *      http://www.apache.org/licenses/LICENSE-2.0                                                                     *
 *                                                                                                                     *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed    *
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.                       *
 * See the License for the specific language governing permissions and limitations under the License.                  *
 * =================================================================================================================== *
 *                                                                                                                     *
 * The following changes were made to the original code:                                                               *
 * - removed unneeded code                                                                                             *
 * - renamed class from hash map to table                                                                              *
 * - renamed symbols to match elvea's coding style                                                                     *
 * - changed keys and values from void* to elvea_variant_t                                                             *
 * - added a thread argument to all methods                                                                            *
 * - changed hash and equality to use elvea's instead of user-provided callbacks                                       *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#include <sys/types.h>
#include <elvea/table.h>
#include <elvea/thread.h>
#include <elvea/utils/alloc.h>

typedef struct table_node_t table_node_t;

struct table_node_t
{
	elvea_variant_t key;
	elvea_variant_t value;
	table_node_t *next;
	elvea_size_t hash;
};

struct elvea_table_t
{
	elvea_object_t base;
	table_node_t **buckets;
	elvea_size_t capacity;
	elvea_size_t size;
};

uint32_t elvea_table_instance_size()
{
	return (uint32_t) sizeof(elvea_table_t);
}

elvea_table_t *elvea_table_new(elvea_thread_t *thread, elvea_size_t initial_capacity)
{
	elvea_table_t *self = (elvea_table_t *) elvea_new(thread, thread->table_class, true, 0);

	if (! elvea_check_memory(thread, self)) {
		return NULL;
	}

	// 0.75 load factor.
	elvea_size_t minimum_capacity = initial_capacity * 4 / 3;
	self->capacity = 1;
	self->size = 0;

	while (self->capacity <= minimum_capacity)
	{
		// Bucket count must be power of 2.
		self->capacity <<= 1;
	}
	self->buckets = (table_node_t **) elvea_calloc(thread, self->capacity, sizeof(table_node_t *));

	if (! elvea_check_memory(thread, self->buckets))
	{
		elvea_free(thread, self);
		return NULL;
	}

	return self;
}

size_t elvea_table_length(elvea_thread_t *thread, elvea_table_t *map)
{
	return map->size;
}

static inline
elvea_size_t calculate_index(elvea_size_t bucket_count, elvea_size_t hash)
{
	return hash & (bucket_count - 1);
}

static void expand_if_necessary(elvea_thread_t *thread, elvea_table_t *self)
{
	// If the load factor exceeds 0.75...
	if (self->size > (self->capacity * 3 / 4))
	{
		// Start off with a 0.33 load factor.
		elvea_size_t new_capacity = self->capacity << 1;
		table_node_t **new_buckets = (table_node_t **) elvea_calloc(thread, new_capacity, sizeof(table_node_t *));

		if (! elvea_check_memory(thread, new_buckets)) {
			return; // abort expansion.
		}

		// Move over existing entries.
		for (elvea_size_t i = 0; i < self->capacity; i++)
		{
			table_node_t *entry = self->buckets[i];
			while (entry != NULL)
			{
				table_node_t *next = entry->next;
				size_t index = calculate_index(new_capacity, entry->hash);
				entry->next = new_buckets[index];
				new_buckets[index] = entry;
				entry = next;
			}
		}
		// Copy over internals.
		free(self->buckets);
		self->buckets = new_buckets;
		self->capacity = new_capacity;
	}
}

void elvea_table_finalize(elvea_thread_t *thread, elvea_table_t *self)
{
	for (elvea_size_t i = 0; i < self->capacity; i++)
	{
		table_node_t *entry = self->buckets[i];
		while (entry != NULL)
		{
			table_node_t *next = entry->next;
			free(entry);
			entry = next;
		}
	}

	elvea_free(thread, self->buckets);
	// Don't free table itself.
}

static table_node_t *create_entry(elvea_thread_t *thread, elvea_variant_t *key, elvea_size_t hash, elvea_variant_t *value)
{
	table_node_t *entry = (table_node_t*) elvea_calloc(thread, 1, sizeof(table_node_t));

	if (! elvea_check_memory(thread, entry)) {
		return NULL;
	}

	elvea_copy(thread, &entry->key, key);
	elvea_copy(thread, &entry->value, value);
	entry->hash = hash;
	//entry->next = NULL;

	return entry;
}

static inline
bool equal_keys(elvea_thread_t *thread, elvea_variant_t* key1, elvea_size_t hash1, elvea_variant_t *key2, elvea_size_t hash2)
{
	if (key1 == key2) {
		return true;
	}
	if (hash1 != hash2) {
		return false;
	}
	return elvea_equal(thread, key1, key2);
}

void elvea_table_set(elvea_thread_t *thread, elvea_table_t *self, elvea_variant_t *key, elvea_variant_t *value)
{
	elvea_size_t hash = elvea_hash(thread, key);
	size_t index = calculate_index(self->capacity, hash);
	table_node_t **p = &(self->buckets[index]);

	while (true)
	{
		table_node_t *current = *p;

		// Add a new entry.
		if (current == NULL)
		{
			*p = create_entry(thread, key, hash, value);

			if (*p != NULL)
			{
				self->size++;
				expand_if_necessary(NULL, self);
			}

			return;
		}

		// Replace existing entry.
		if (equal_keys(thread, &current->key, current->hash, key, hash))
		{
			elvea_copy(thread, &current->value, value);
			return;
		}
		// Move to next entry.
		p = &current->next;
	}
}

elvea_variant_t * elvea_table_get(elvea_thread_t *thread, elvea_table_t *self, elvea_variant_t *key)
{
	elvea_size_t hash = elvea_hash(thread, key);
	elvea_size_t index = calculate_index(self->capacity, hash);
	table_node_t *entry = self->buckets[index];

	while (entry != NULL)
	{
		if (equal_keys(thread, &entry->key, entry->hash, key, hash))
		{
			return &entry->value;
		}
		entry = entry->next;
	}

	return NULL;
}

bool elvea_table_contains(elvea_thread_t *thread, elvea_table_t *self, elvea_variant_t *key)
{
	return elvea_table_get(thread, self, key) != NULL;
}

bool elvea_table_remove(elvea_thread_t *thread, elvea_table_t *self, elvea_variant_t *key)
{
	elvea_size_t hash = elvea_hash(thread, key);
	elvea_size_t index = calculate_index(self->capacity, hash);
	// Pointer to the current entry.
	table_node_t **p = &(self->buckets[index]);
	table_node_t *current;

	while ((current = *p) != NULL)
	{
		if (equal_keys(thread, &current->key, current->hash, key, hash))
		{
			*p = current->next;
			elvea_release(thread, &current->key);
			elvea_release(thread, &current->value);
			elvea_free(thread, current);
			self->size--;

			return true;
		}
		p = &current->next;
	}

	return false;
}

void elvea_table_apply(elvea_thread_t *thread, elvea_table_t *map, bool (*callback)(elvea_variant_t *, elvea_variant_t *, void *), void *context)
{
	for (elvea_size_t i = 0; i < map->capacity; i++)
	{
		table_node_t *entry = map->buckets[i];

		while (entry != NULL)
		{
			table_node_t *next = entry->next;
			if (!callback(&entry->key, &entry->value, context))
			{
				return;
			}
			entry = next;
		}
	}
}

// Is this useful for elvea?
elvea_size_t elvea_table_current_capacity(elvea_table_t *self)
{
	elvea_size_t bucketCount = self->capacity;
	return bucketCount * 3 / 4;
}

elvea_size_t elvea_table_count_collisions(elvea_table_t *self)
{
	elvea_size_t collisions = 0;

	for (elvea_size_t i = 0; i < self->capacity; i++)
	{
		table_node_t *entry = self->buckets[i];
		while (entry != NULL)
		{
			if (entry->next != NULL)
			{
				collisions++;
			}
			entry = entry->next;
		}
	}

	return collisions;
}
