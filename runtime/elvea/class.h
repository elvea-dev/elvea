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
 * Purpose: implement an elvea type.                                                                                   *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_CLASS_H
#define ELVEA_CLASS_H

#include <elvea/definitions.h>

#ifdef __cplusplus
extern "C" {
#endif


struct elvea_class_t
{
	// User-readable name for the type.
	const char *name;

	// Base classes. In case of ambiguous lookup, the class with the lowest index takes precedence.
	elvea_class_t *bases[ELVEA_MAX_BASE_COUNT];

	// Number of base classes.
	uint32_t base_count;

	// Basic size for an instance of the type. Some types (such as string) allocate more space
	// at the end of the struct to store their data.
	uint32_t alloc_size;

	// Chain classes together so that we can easily keep track of all registered classes.
	elvea_class_t *next;
	
	// Generic methods.
	elvea_finalize_callback_t finalize;
	elvea_clone_callback_t clone;
	elvea_traverse_callback_t traverse;
	elvea_compare_callback_t compare;
	elvea_equal_callback_t equal;
	elvea_hash_callback_t hash;
	elvea_iterate_callback_t iterate;
};


//----------------------------------------------------------------------------------------------------------------------

elvea_class_t *elvea_class_new(elvea_thread_t *thread, const char *name, uint32_t size, uint32_t base_count, elvea_class_t **bases);


#ifdef __cplusplus
}
#endif

#endif // ELVEA_CLASS_H
