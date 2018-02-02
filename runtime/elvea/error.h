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
 * Purpose: elvea errors.                                                                                              *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_ERROR_H
#define ELVEA_ERROR_H

#include <elvea/definitions.h>


#ifdef __cplusplus
extern "C" {
#endif

enum
{
	ELVEA_ERROR_ANY,            /*!< Generic error */
	ELVEA_ERROR_ACCESS,         /*!< Member privacy violation */
	ELVEA_ERROR_INTERNAL,       /*!< Internal error */
	ELVEA_ERROR_IMPORT,         /*!< Module import error */
	ELVEA_ERROR_ASSERTION,      /*!< Assertion failure */
	ELVEA_ERROR_TYPE,           /*!< Type error */
	ELVEA_ERROR_CAST,           /*!< Type conversion error */
	ELVEA_ERROR_FILESYSTEM,     /*!< File system access error */
	ELVEA_ERROR_INDEX,          /*!< Invalid index */
	ELVEA_ERROR_RUNTIME,        /*!< Runtime error */
	ELVEA_ERROR_MEMORY,         /*!< Out of memory */
	ELVEA_ERROR_MATH,           /*!< Math error */
	ELVEA_ERROR_UNICODE,        /*!< Unicode error */
	ELVEA_ERROR_REGEX,          /*!< Invalid regular expression */
	ELVEA_ERROR_SYNTAX,         /*!< Syntax error */
	ELVEA_ERROR_SYMBOL,         /*!< Symbol error */
	ELVEA_ERROR_IO,             /*!< Input/output error */
	ELVEA_ERROR_UNIMPLEMENTED   /*!< Unimplemented feature */
};


// Throw an error.
void elvea_throw(elvea_thread_t *thread, int code, const char *fmt, ...);

const char *elvea_error_name(int code);


#ifdef __cplusplus
}
#endif

#endif //ELVEA_ERROR_H
