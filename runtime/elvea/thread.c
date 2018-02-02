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
 ***********************************************************************************************************************/

#include <stddef.h>
#include <elvea/elvea.h>
#include <elvea/utils/alloc.h>

void elvea_thread_init(elvea_runtime_t *rt, elvea_thread_t *thread)
{
	thread->seed = (uint32_t) rand();
	thread->runtime = rt;
	thread->has_thread = false;
	thread->main_thread = false;
	thread->next = NULL;
	elvea_gc_initialize(&thread->gc);

	thread->bool_class   = elvea_class_new(thread, "bool", 0, 0, NULL);
	thread->num_class    = elvea_class_new(thread, "num", 0, 0, NULL);
	thread->string_class = elvea_class_new(thread, "string", (uint32_t) offsetof(elvea_string_t, data), 0, NULL);
	thread->table_class  = elvea_class_new(thread, "table", elvea_table_instance_size(), 0, NULL);
	thread->iter_class   = elvea_class_new(thread, "iterator", sizeof(elvea_iterator_t), 0, NULL);

	elvea_string_init_class(thread->string_class);
}

void elvea_thread_delete(elvea_thread_t *thread)
{
	if (thread == NULL) {
		return;
	}

	elvea_thread_delete(thread->next);
	elvea_gc_finalize(&thread->gc);
	elvea_free(thread, thread->bool_class);
	elvea_free(thread, thread->num_class);
	elvea_free(thread, thread->string_class);
	elvea_free(thread, thread->table_class);
	elvea_free(thread, thread->iter_class);

	thread->runtime->alloc(thread, 0, 0);
}

elvea_variant_t * elvea_alloc_variant(elvea_thread_t *thread)
{
	return elvea_arena_alloc_variant(thread, &thread->gc.arena);
}

void elvea_recycle_variant(elvea_thread_t *thread, elvea_variant_t *variant)
{
	elvea_arena_recycle_variant(thread, &thread->gc.arena, variant);
}

elvea_alias_t * elvea_alloc_alias(elvea_thread_t *thread)
{
	return elvea_arena_alloc_alias(thread, &thread->gc.arena);
}

void elvea_recycle_alias(elvea_thread_t *thread, elvea_alias_t *alias)
{
	elvea_arena_recycle_alias(thread, &thread->gc.arena, alias);
}
