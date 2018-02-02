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
 * Purpose: see header.                                                                                                *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#include <string.h>
#include <elvea/error.h>
#include <elvea/runtime.h>
#include <elvea/thread.h>
#include <elvea/utils/alloc.h>

void *elvea_alloc(elvea_thread_t *thread, size_t size)
{
	return thread->runtime->alloc(NULL, 0, size);
}

void *elvea_calloc(elvea_thread_t *thread, size_t count, size_t size)
{
	size = count * size;
	void *data = thread->runtime->alloc(NULL, 0, size);

	if (data) {
		memset(data, 0, size);
	}

	return data;
}

void *elvea_realloc(elvea_thread_t *thread, void *ptr, size_t size)
{
	return thread->runtime->alloc(ptr, 0, size);
}


void elvea_free(elvea_thread_t *thread, void *ptr)
{
	thread->runtime->alloc(ptr, 0, 0);
}
