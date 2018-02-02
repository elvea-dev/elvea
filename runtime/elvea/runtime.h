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
 * Purpose: global runtime, which stores global parameters. There is only one runtime per application.                 *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_RUNTIME_H
#define ELVEA_RUNTIME_H

#include <elvea/definitions.h>

#ifdef __cplusplus
extern "C" {
#endif



struct elvea_runtime_t
{
	// Memory allocator.
	elvea_allocator_t alloc;

	// If this is non-NULL, errors are forwarded to this handler, otherwise a default handler is used.
	elvea_error_callback_t error_handler;

	// Main thread.
	elvea_thread_t *thread;
};


elvea_thread_t *elvea_initialize(elvea_runtime_t *runtime, elvea_allocator_t alloc, elvea_error_callback_t error_handler);

void elvea_finalize(elvea_runtime_t *runtime);


#ifdef __cplusplus
}
#endif

#endif // ELVEA_RUNTIME_H
