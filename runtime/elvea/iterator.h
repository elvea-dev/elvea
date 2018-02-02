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
 * Purpose: implements an iterator to an iterable object (e.g. string, list, table).                                   *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_ITERATOR_H
#define ELVEA_ITERATOR_H

#include <elvea/definitions.h>
#include <elvea/variant.h>

#ifdef __cplusplus
extern "C" {
#endif

struct elvea_iterator_t
{
	// Common base.
	elvea_object_t base;

	// Iterators keep an alias to the object to ensure that the object remains valid.
	elvea_alias_t *alias;

	// Direct reference to the object being iterated.
	elvea_object_t *object;

	// Any additional information needed by the alias. For example, a string iterator
	// stores a byte offset in the string.
	elvea_variant_t data;
};


//----------------------------------------------------------------------------------------------------------------------

// Create a new iterator for an iterable object.
elvea_iterator_t *elvea_iterator_new(elvea_thread_t *thread, elvea_variant_t *target);

// Get the next element from the iterable object and store it in [result]. Returns true if an element was obtained,
// and false otherwise.
bool elvea_iterator_next(elvea_thread_t *thread, elvea_iterator_t *self, elvea_variant_t *result);


#ifdef __cplusplus
}
#endif

#endif // ELVEA_ITERATOR_H
