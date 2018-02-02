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
 * Created: 2018.02.02                                                                                                 *
 *                                                                                                                     *
 * Purpose: see header.                                                                                                *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#if !defined(__APPLE__)
#include <malloc.h>
#endif

#include <time.h>
#include <elvea/runtime.h>
#include <elvea/thread.h>

static void* default_alloc(void* ptr, size_t old_size, size_t new_size)
{
	if (new_size == 0)
	{
		free(ptr);
		return NULL;
	}

	return realloc(ptr, new_size);
}


//----------------------------------------------------------------------------------------------------------------------

elvea_thread_t * elvea_initialize(elvea_runtime_t *runtime, elvea_allocator_t alloc, elvea_error_callback_t error_handler)
{
	srand((unsigned int) time(NULL));
	runtime->alloc = (alloc == NULL) ? default_alloc : alloc;

	elvea_thread_t *main_thread = (elvea_thread_t*) runtime->alloc(NULL, 0, sizeof(elvea_thread_t));

	if (main_thread) {
		elvea_thread_init(runtime, main_thread);
	}
	runtime->thread = main_thread;

	return main_thread;
}

void elvea_finalize(elvea_runtime_t *runtime)
{
	elvea_thread_delete(runtime->thread);
}
