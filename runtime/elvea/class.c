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

#include <string.h>
#include <elvea/class.h>
#include <elvea/utils/alloc.h>


elvea_class_t *elvea_class_new(elvea_thread_t *thread, const char *name, uint32_t size, uint32_t base_count, elvea_class_t **bases)
{
	elvea_class_t *self = (elvea_class_t*) elvea_calloc(thread, 1, sizeof(elvea_class_t));

	self->name = name;
	self->alloc_size = size;
	self->base_count = base_count;

	if (bases != NULL)
	{
		memcpy(self->bases, bases, sizeof(elvea_class_t*) * base_count);
	}

	return self;
}