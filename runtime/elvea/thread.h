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
 * Purpose: a thread roughly models a native thread. There should be a one-to-one mapping between elvea threads and    *
 * native threads. Each thread stores its own data locally, including its one garbage collector and memory arena.      *
 * Communication between threads is done via message passing.                                                          *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_THREAD_H
#define ELVEA_THREAD_H

#include <elvea/gc.h>
#include <elvea/error.h>
#include <elvea/third_party/tinycthread/tinycthread.h>


#ifdef __cplusplus
extern "C" {
#endif



struct elvea_thread_t
{
	// Random seed for hashing.
	uint32_t seed;

	// Thread-local garbage collector.
	struct elvea_recycler_t gc;

	// Global runtime.
	elvea_runtime_t *runtime;

	// Native thread associated with this thread, if any.
	thrd_t native_thread;

	// Whether a native thread is associated with this elvea thread.
	bool has_thread;

	// Whether this is the main thread, which is created by the runtime.
	bool main_thread;

	// Chain threads together.
	elvea_thread_t *next;

	// Builtin classes.
	elvea_class_t *bool_class;
	elvea_class_t *num_class;
	elvea_class_t *string_class;
	elvea_class_t *list_class;
	elvea_class_t *table_class;
	elvea_class_t *iter_class;


};


//----------------------------------------------------------------------------------------------------------------------

// Initialize runtime.
void elvea_thread_init(elvea_runtime_t *rt, elvea_thread_t *thread);

// Finalize runtime.
void elvea_thread_delete(elvea_thread_t *thread);

// Get an uninitialized storage area large enough to hold a variant.
elvea_variant_t * elvea_alloc_variant(elvea_thread_t *thread);

// Put back variant into the arena.
void elvea_recycle_variant(elvea_thread_t *thread, elvea_variant_t *variant);

// Get an uninitialized storage area large enough to hold an alias.
elvea_alias_t * elvea_alloc_alias(elvea_thread_t *thread);

// Put back alias into the arena.
void elvea_recycle_alias(elvea_thread_t *thread, elvea_alias_t *alias);

#ifdef __cplusplus
}
#endif

#endif // ELVEA_THREAD_H
