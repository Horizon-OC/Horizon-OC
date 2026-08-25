#include "fslc_string.h"
#include <stdint.h>

void *fslc_memcpy(void *dest, const void *src, size_t len)
{
    /* Copy byte-by-byte up to word-aligned.
     *
     * There's a possibility, that src and dest have different offsets
     * to word boundary and can not both be used efficiently.
     *
     * In this case we use align to dest argument, because it then can be
     * written in one operation, instead of read-modify-write of 2 words.
     *
     * Most of the time both parameters should be word-aligned, however.
     */
    unsigned char *b_src = (unsigned char *)src;
    unsigned char *b_dst = (unsigned char *)dest;
    for (;((uintptr_t)b_dst & (sizeof(unsigned long)-1)) && len > 0; b_src++, b_dst++, len--)
    {
        *b_dst = *b_src;
    }

    /* Then all word-aligned */
    unsigned long *w_src  = (unsigned long*)b_src;
    unsigned long *w_dst = (unsigned long*)b_dst;
    for (;len >= sizeof(unsigned long); w_src++, w_dst++, len -= sizeof(unsigned long))
    {
        *w_dst = *w_src;
    }

    /* And byte-by-byte the rest */
    b_src  = (unsigned char *)w_src;
    b_dst = (unsigned char *)w_dst;
    for (; len > 0; b_src++, b_dst++, len--)
    {
        *b_dst = *b_src;
    }

    return dest;
}
