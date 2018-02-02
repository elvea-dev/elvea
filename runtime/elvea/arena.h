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
 * Purpose: memory pool (or "arena") for variants and aliases.                                                         *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_ARENA_H
#define ELVEA_ARENA_H

#include <elvea/variant.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct elvea_heap_page_t elvea_heap_page_t;
typedef struct elvea_arena_t elvea_arena_t;
struct elvea_heap_slot_t;

struct elvea_arena_t
{
	struct elvea_heap_slot_t *free_list;
	elvea_heap_page_t *pages;
};


//----------------------------------------------------------------------------------------------------------------------

// Get an uninitialized storage area large enough to hold a variant.
elvea_variant_t *elvea_arena_alloc_variant(elvea_thread_t *thread, elvea_arena_t *arena);

// Put back variant into the arena.
void elvea_arena_recycle_variant(elvea_thread_t *thread, elvea_arena_t *arena, elvea_variant_t *variant);

// Get an uninitialized storage area large enough to hold an alias.
elvea_alias_t * elvea_arena_alloc_alias(elvea_thread_t *thread, elvea_arena_t *arena);

// Put back alias into the arena.
void elvea_arena_recycle_alias(elvea_thread_t *thread, elvea_arena_t *arena, elvea_alias_t *alias);

// Remove memory pages which are unused.
void elvea_arena_compact(elvea_thread_t *thread, elvea_arena_t *arena);


#ifdef __cplusplus
}
#endif

#endif // ELVEA_ARENA_H
