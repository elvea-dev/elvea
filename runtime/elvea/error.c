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
 * Created: 2017.12.25                                                                                                 *
 *                                                                                                                     *
 * Purpose: see header.                                                                                                *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <elvea/error.h>
#include <elvea/thread.h>
#include <elvea/runtime.h>

static const char *error_msg[] = {
	"Error",
	"Access error",
	"Internal error",
	"Import error",
	"Assertion error",
	"Type error",
	"Cast error",
	"File system error",
	"Index error",
	"Runtime error",
	"Memory error",
	"Math error",
	"Unicode error",
	"Regular expression error",
	"Syntax error",
	"Symbol error",
	"Input/Output error",
	"Feature not implemented"
};

void elvea_throw(elvea_thread_t *thread, int code, const char *fmt, ...)
{
	char buffer[ELVEA_ERROR_BUFFER_SIZE];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buffer, ELVEA_ERROR_BUFFER_SIZE, fmt, args);
	va_end(args);

	if (thread->runtime->error_handler)
	{
		thread->runtime->error_handler(code, buffer);
	}
	else
	{
		fprintf(stderr, "[%s]\n%s\n", elvea_error_name(code), buffer);
		exit(100 + code); // TODO: handle exceptions
	}
}


const char *elvea_error_name(int code)
{
	return error_msg[code];
}