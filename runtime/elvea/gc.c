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
 * Created: 2018.02.01                                                                                                 *
 *                                                                                                                     *
 * Purpose: see header.                                                                                                *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#include <stddef.h>
#include <elvea/class.h>
#include <elvea/gc.h>
#include <elvea/thread.h>
#include <elvea/error.h>
#include <elvea/utils/alloc.h>

/*
 * Header for all objects managed by the garbage collector. Managed objects have the following layout in memory:
 *
 *      gc root ->  -------------------
 *                 |  elvea_gc_object_t |
 *  object root ->  -------------------
 *                 |   elvea_object_t   |
 *                  -------------------
 *                 | other fields for  |
 *                 | derived classes   |
 *                  -------------------
 *
 *  When the GC allocates a managed object, it adds a elvea_gc_object_t right before the object and returns the object
 *  itself. Unmanaged objects are allocated directly, without this header. Since objects never move once they have
 *  been allocated, we can always get the gc root from the object root, and vice versa, by simply moving the pointer
 *  by an offset of sizeof(elvea_gc_object_t) bytes. Whether an object is managed or not is controlled by the `collectable`
 *  flag in the elvea_object_t header.
 */

// Header for collectable objects.
struct elvea_gc_object_t
{
	// GC chain for collectable objects. The list is doubly-linked so that objects can detach themselves from the chain.
	struct elvea_gc_object_t *previous, *next;

	// Access root of the object from the GC header.
	elvea_object_t object;
};


// Size of the header for collectable objects.
#define GC_OBJECT_SIZE offsetof(struct elvea_gc_object_t, object)

// Get the start of a collectable object's header.
#define GET_GC_OBJECT(ptr) ((struct elvea_gc_object_t*) (((char*)(ptr)) - GC_OBJECT_SIZE))


void *elvea_new(elvea_thread_t *thread, elvea_class_t *type, bool collectable, int extra)
{
	elvea_object_t *self;

	if (collectable)
	{
		// Allocate object preceded by a header for the GC.
		size_t byte_count = type->alloc_size + extra + GC_OBJECT_SIZE;
		struct elvea_gc_object_t *gc_object = (struct elvea_gc_object_t*) elvea_alloc(thread, byte_count);

		// Attach object to the GC chain.
		struct elvea_gc_object_t *old_root = thread->gc.root;
		gc_object->next = old_root;
		if (old_root) old_root->previous = gc_object;
		thread->gc.root = gc_object;

		// Get pointer to the actual object.
		self = &gc_object->object;
	}
	else
	{
		size_t byte_count = type->alloc_size + extra;
		self = (elvea_object_t*) elvea_alloc(thread, byte_count);
	}

	self->isa = type;
	self->meta.ref_count = 0;
	self->meta.gc_color = collectable ? ELVEA_GC_BLACK : ELVEA_GC_GREEN;
	self->meta.flags = 0;

	return self;
}

void elvea_delete(elvea_thread_t *thread, void *ptr)
{
	elvea_object_t *self = (elvea_object_t*) ptr;
	elvea_finalize_callback_t finalize = self->isa->finalize;

	if (elvea_is_collectable(self))
	{
		// Get pointer to the object's header.
		struct elvea_gc_object_t *gc_object = GET_GC_OBJECT(ptr);
		ptr = gc_object;

		// Detach object from the GC chain.
		if (gc_object == thread->gc.root)
		{
			thread->gc.root = gc_object->next;
		}

		if (gc_object->previous)
		{
			gc_object->previous->next = gc_object->next;
		}
		if (gc_object->next)
		{
			gc_object->next->previous = gc_object->previous;
		}
	}

	// Release resources managed by the object.
	if (finalize)
	{
		finalize(thread, self);
	}

	// Now the object can be freed.
	elvea_free(thread, ptr);
}

void elvea_object_release(elvea_thread_t *thread, void *ptr)
{
	if (elvea_unref(ptr))
	{
		elvea_delete(thread, ptr);
	}
	else if (elvea_is_collectable(ptr))
	{
		// Potentially cyclic object.
		((elvea_object_t*)ptr)->meta.gc_color = ELVEA_GC_PURPLE;
	}
}

void elvea_alias_release(elvea_thread_t *thread, elvea_alias_t *alias)
{
	if (elvea_unref(alias))
	{
		elvea_release(thread, &alias->variant);
		elvea_recycle_alias(thread, alias);
	}
}

//----------------------------------------------------------------------------------------------------------------------

bool elvea_check_memory(elvea_thread_t *thread, void *ptr)
{
	if (ptr == NULL)
	{
		elvea_throw(thread, ELVEA_ERROR_MEMORY, "memory allocation failed");
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------------------------------------------

void elvea_gc_initialize(elvea_recycler_t *gc)
{
	// TODO : init GC
}

void elvea_gc_finalize(elvea_recycler_t *gc)
{

}