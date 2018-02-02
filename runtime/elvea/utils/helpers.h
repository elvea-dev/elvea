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
 * Created: 2017.12.26                                                                                                 *
 *                                                                                                                     *
 * Purpose: helper routines.                                                                                           *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#ifndef ELVEA_HELPERS_H
#define ELVEA_HELPERS_H

#include <elvea/definitions.h>

// Use byte swapping primitives if available, otherwise default to generic versions.

#if defined(_WIN32) && !defined(ELVEA_MINGW)
#include <intrin.h>
#   define ELVEA_BYTESWAP16(x) _byteswap_ushort(x)
#   define ELVEA_BYTESWAP32(x) _byteswap_ulong(x)
#   define ELVEA_HAS_BYTESWAP
#elif defined(__APPLE__)
#    include <libkern/OSByteOrder.h>
#   define ELVEA_BYTESWAP16(x) OSSwapInt16(x)
#   define ELVEA_BYTESWAP32(x) OSSwapInt32(x)
#   define ELVEA_HAS_BYTESWAP
#   define ELVEA_ENDIANNES_KNOWN
#elif (defined(__GNUC__) || defined(__GNUG__)) && !defined(ELVEA_MINGW)
#    include <byteswap.h>
#   define ELVEA_BYTESWAP16(x) __bswap_16(x)
#   define ELVEA_BYTESWAP32(x) __bswap_32(x)
#   define ELVEA_HAS_BYTESWAP
#   define ELVEA_ENDIANNES_KNOWN
#else
#   define ELVEA_BYTESWAP16(x) ((x >> 8) | (x << 8))
#   define ELVEA_BYTESWAP32(x) \
    ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
    (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
#endif


#ifdef __cplusplus
extern "C" {
#endif


static inline
elvea_size_t get_next_capacity(elvea_size_t n)
{
	assert(n != 0);
	return (n < 32) ? (n << 1) : (n + (n >> 1));
}


#ifdef __cplusplus
}
#endif

#endif /* ELVEA_HELPERS_H */
