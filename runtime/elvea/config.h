/***********************************************************************************************************************
 *                                                                                                                     *
 * Copyright (C) 2018 Julien Eychenne                                                                                  *
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
 * Created: 2018.01.12                                                                                                 *
 *                                                                                                                     *
 * Purpose: user-definable macros.                                                                                     *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_CONFIG_H
#define ELVEA_CONFIG_H

// Number of slots in a memory page (see arena.h).
#define ELVEA_PAGE_SIZE 1024

// Maximum number of base classes for a class.
#define ELVEA_MAX_BASE_COUNT 8

#define ELVEA_ERROR_BUFFER_SIZE 256

#endif // ELVEA_CONFIG_H
