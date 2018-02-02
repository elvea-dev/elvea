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
 * Purpose: backup garbage collector which detects reference cycles. The algorithm implements the synchronous recycler *
 * described in _The Garbage Collection Handbook. The Art of Automatic Memory Management_, by R. Jones, A. Hosking &   *
 * E. Moss. (See p. 66 ff.). This algorithm was first proposed by Bacon & Rajan (2001).                                *
 * This implementation distinguishes between collectable and non-collectable objects. Non-collectable objects are      *
 * never candidates for garbage collection since they can't contain reference cycles (e.g. string). Collectable        *
 * objects become candidates as soon as there is more that one reference pointing to them, since they might contain    *
 * a cycle; however, if the object's reference count reaches 0, the object is detached from the GC chain and it is     *
 * destroyed.                                                                                                          *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_RECYCLER_H
#define ELVEA_RECYCLER_H

#include <elvea/definitions.h>
#include <elvea/arena.h>

#ifdef __cplusplus
extern "C" {
#endif

struct elvea_gc_object_t;


struct elvea_recycler_t
{
	// Memory arena for heap-allocated variants and aliases.
	elvea_arena_t arena;

	// Root of the GC chain. This is a doubly-linked list so that objects can detach themselves from the list
	// when their reference count reaches 0. This is fine as long as the runtime is kept in a single thread.
	struct elvea_gc_object_t *root;
};


//----------------------------------------------------------------------------------------------------------------------

void elvea_gc_initialize(elvea_recycler_t *gc);

void elvea_gc_finalize(elvea_recycler_t *gc);

#ifdef __cplusplus
}
#endif

#endif // ELVEA_RECYCLER_H
