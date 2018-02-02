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
 * Purpose: a dynamic hash table which maps key/value pairs. Keys and values are variants, and as such they can store  *
 * any type supported by the elvea engine.                                                                             *
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

#ifndef ELVEA_TABLE_H
#define ELVEA_TABLE_H

#include <elvea/definitions.h>

#ifdef __cplusplus
extern "C" {
#endif

// Get size of an instance.
uint32_t elvea_table_instance_size();


/**
 * Creates a new table. Returns NULL if memory allocation fails.
 *
 * @param initial_capacity number of expected entries
 */
elvea_table_t *elvea_table_new(elvea_thread_t *thread, elvea_size_t initial_capacity);

/**
 * Frees the table. Does not free the keys or values themselves.
 */
void elvea_table_finalize(elvea_thread_t *thread, elvea_table_t *self);

/**
 * Puts value for the given key in the map. Returns pre-existing value if
 * any.
 *
 * If memory allocation fails, this function returns NULL, the map's size
 * does not increase, and errno is set to ENOMEM.
 */
void elvea_table_set(elvea_thread_t *thread, elvea_table_t *self, elvea_variant_t *key, elvea_variant_t *value);

/**
 * Gets a value from the map. Returns NULL if no entry for the given key is
 * found or if the value itself is NULL.
 */
elvea_variant_t * elvea_table_get(elvea_thread_t *thread, elvea_table_t *self, elvea_variant_t *key);
/**
 * Returns true if the map contains an entry for the given key.
 */
bool elvea_table_contains(elvea_thread_t *thread, elvea_table_t *self, elvea_variant_t *key);

/**
 * Removes an entry from the map. Returns true if the value was removed value and false
 * otherwise.
 */
bool elvea_table_remove(elvea_thread_t *thread, elvea_table_t *self, elvea_variant_t *key);

/**
 * Gets the number of entries in this map.
 */
size_t elvea_table_length(elvea_thread_t *thread, elvea_table_t *map);

/**
 * Invokes the given callback on each entry in the map. Stops iterating if
 * the callback returns false.
 */
void elvea_table_apply(elvea_thread_t *thread, elvea_table_t *map, bool (*callback)(elvea_variant_t *, elvea_variant_t *, void *),
					  void *context);


//----------------------------------------------------------------------------------------------------------------------

/**
 * For debugging.
 */

/**
 * Gets current capacity.
 */
elvea_size_t elvea_table_current_capacity(elvea_table_t *self);

/**
 * Counts the number of entry collisions.
 */
elvea_size_t elvea_table_count_collisions(elvea_table_t *self);

#ifdef __cplusplus
}
#endif

#endif /* ELVEA_TABLE_H */
