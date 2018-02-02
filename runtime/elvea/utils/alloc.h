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
 * Created: 2017.01.17                                                                                                 *
 *                                                                                                                     *
 * Purpose: memory allocation routines. These functions are similar to malloc/calloc/realloc/free but use the          *
 * runtime's allocator (which, by default, uses realloc and free).                                                     *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_ALLOC_H
#define ELVEA_ALLOC_H

#include <stddef.h>
#include <elvea/definitions.h>

#ifdef __cplusplus
extern "C" {
#endif


// Allocate a block of uninitialized memory.
void *elvea_alloc(elvea_thread_t *thread, size_t size);

// Allocate a block of zero-initialized memory.
void *elvea_calloc(elvea_thread_t *thread, size_t count, size_t size);

// Reallocate a block of memory.
void *elvea_realloc(elvea_thread_t *thread, void *ptr, size_t size);

// Free a block of memory.
void elvea_free(elvea_thread_t *thread, void *ptr);


#ifdef __cplusplus
}
#endif

#endif // ELVEA_ALLOC_H
