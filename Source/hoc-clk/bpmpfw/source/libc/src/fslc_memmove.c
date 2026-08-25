#include "fslc_string.h"
#include <stdint.h>

void *fslc_memmove(void *dest, const void *src, size_t len)
{
    unsigned char *b_dst = (unsigned char *)dest;
    unsigned char *b_src = (unsigned char *)src;

    /* Need to check if memory regions overlaps. Then if the dest address is
     * larger than source, we need to copy backwards (or else source regions
     * ending will be overwritten by writes to dest, before they are read).
     *
     * In other cases it works identically as memcpy(). Same considerations
     * about address alignment applies here.
     */
    if ( b_dst < b_src + len && b_dst > b_src)
    {
        b_src += len;
        b_dst += len;

        /* Byte-by-byte down to word-aligned. */
        for (;((uintptr_t)b_dst & (sizeof(unsigned long)-1)) && len > 0; )
        {
            b_src--, b_dst--, len--;
            *b_dst = *b_src;
        }

        /* Then all word-aligned */
        unsigned long *w_src  = (unsigned long*)b_src;
        unsigned long *w_dst = (unsigned long*)b_dst;
        for (;len >= sizeof(unsigned long); )
        {
            w_src--, w_dst--, len -= sizeof(unsigned long);
            *w_dst = *w_src;
        }

        /* And byte-by-byte the rest */
        b_src = (unsigned char *)w_src;
        b_dst = (unsigned char *)w_dst;
        for (; len > 0; )
        {
            b_src--, b_dst--, len--;
            *b_dst = *b_src;
        }
    } else {
        /* Byte-by-byte up to word-aligned. */
        for (;((uintptr_t)b_dst & (sizeof(unsigned long)-1)) && len > 0; b_src++, b_dst++, len--)
        {
            *b_dst = *b_src;
        }

        /* Then all word-aligned */
        unsigned long *w_src  = (unsigned long *)b_src;
        unsigned long *w_dst = (unsigned long *)b_dst;
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
    }
    return dest;
}
