#ifndef ELVEA_TEST_H
#define ELVEA_TEST_H

#include <elvea/elvea.h>
#include "cutest/CuTest.h"

#define GET_RUNTIME(T, tc) elvea_thread_t *T = (elvea_thread_t *) tc->data;

#define STR(name, str) elvea_string_t *name = elvea_string_new(thread, str, -1); elvea_object_retain(thread, name)


#endif // ELVEA_TEST_H
