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

#include <stddef.h>
#include <elvea/arena.h>
#include <elvea/utils/alloc.h>

struct elvea_heap_slot_t
{
	elvea_alias_t data;
	struct elvea_heap_slot_t *next;
};

struct elvea_heap_page_t
{
	elvea_heap_page_t *next;
	struct elvea_heap_slot_t data[ELVEA_PAGE_SIZE];
};

static void link_nodes(elvea_heap_page_t *page)
{
	static const size_t last_index = ELVEA_PAGE_SIZE - 1;

	for (size_t i = 0; i < last_index; ++i)
	{
		page->data[i].next = &page->data[i+1];
	}

	page->data[last_index].next = NULL;
}

static void add_page(elvea_thread_t *thread, elvea_arena_t *arena)
{
	elvea_heap_page_t *page = (elvea_heap_page_t *) elvea_alloc(thread, sizeof(elvea_heap_page_t));
	page->next = arena->pages;
	arena->pages = page;
	link_nodes(page);
	arena->free_list = &page->data[0];
}

static struct elvea_heap_slot_t *get_slot(elvea_thread_t *thread, elvea_arena_t *arena)
{
	if (arena->free_list == NULL) {
		add_page(thread, arena);
	}

	struct elvea_heap_slot_t *slot = arena->free_list;
	arena->free_list = slot->next;
	slot->data.meta.arena = true;

	return slot;
}

static void recycle_slot(elvea_thread_t *thread, elvea_arena_t *arena, struct elvea_heap_slot_t *slot)
{
	slot->next = arena->free_list;
	arena->free_list = slot;
	slot->data.meta.arena = false;
}

elvea_variant_t * elvea_arena_alloc_variant(elvea_thread_t *thread, elvea_arena_t *arena)
{
	struct elvea_heap_slot_t *slot = get_slot(thread, arena);
	return &slot->data.variant;
}

void elvea_arena_recycle_variant(elvea_thread_t *thread, elvea_arena_t *arena, elvea_variant_t *variant)
{
	static const size_t offset = offsetof(elvea_alias_t, variant);
	struct elvea_heap_slot_t *slot = (struct elvea_heap_slot_t *) (((char*)variant) - offset);
	recycle_slot(thread, arena, slot);
}

elvea_alias_t * elvea_arena_alloc_alias(elvea_thread_t *thread, elvea_arena_t *arena)
{
	struct elvea_heap_slot_t *slot = get_slot(thread, arena);
	return &slot->data;
}

void elvea_arena_recycle_alias(elvea_thread_t *thread, elvea_arena_t *arena, elvea_alias_t *alias)
{
	struct elvea_heap_slot_t *slot = (struct elvea_heap_slot_t *) alias;
	recycle_slot(thread, arena, slot);
}

void elvea_arena_compact(elvea_thread_t *thread, elvea_arena_t *arena)
{
	elvea_heap_page_t **page_address = &arena->pages;

	while (*page_address != NULL)
	{
		elvea_heap_page_t *page = *page_address;
		bool used = false;
		struct elvea_heap_slot_t *slots = &page->data[0];

		for (size_t i = 0; i < ELVEA_PAGE_SIZE; ++i)
		{
			if (slots[i].data.meta.arena)
			{
				used = true;
				break;
			}
		}

		if (!used)
		{
			// Remove page items from the free list.
			struct elvea_heap_slot_t *start_address = &page->data[0];
			struct elvea_heap_slot_t *end_address = &slots[ELVEA_PAGE_SIZE-1];
			struct elvea_heap_slot_t **slot_address = &arena->free_list;

			while (*slot_address)
			{
				struct elvea_heap_slot_t *slot = *slot_address;

				if (start_address <= slot && slot <= end_address)
				{
					// Detach node
					*slot_address = slot->next;
				}
				else
				{
					// Move to next node
					slot_address = &slot->next;
				}
			}

			*page_address = page->next;
			elvea_free(thread, page_address);
		}
	}
}
