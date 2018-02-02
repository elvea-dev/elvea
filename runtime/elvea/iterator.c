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

#include <elvea/iterator.h>
#include <elvea/thread.h>


elvea_iterator_t *elvea_iterator_new(elvea_thread_t *thread, elvea_variant_t *target)
{
	elvea_iterator_t *self = (elvea_iterator_t*) elvea_new(thread, thread->iter_class, false, 0);

	if (! elvea_check_memory(thread, self)) {
		return NULL;
	}

	elvea_make_alias(thread, target);
	elvea_alias_t *alias = target->as.alias;
	self->alias = alias;
	self->object = alias->variant.as.object;
	elvea_alias_retain(thread, alias);
	elvea_zero(&self->data);

	return self;
}

bool elvea_iterator_next(elvea_thread_t *thread, elvea_iterator_t *self, elvea_variant_t *result)
{
	elvea_iterate_callback_t iterate = self->object->isa->iterate;
	assert(iterate != NULL);
	return iterate(thread, self->object, &self->data, result);
}