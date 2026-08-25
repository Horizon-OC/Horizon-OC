#include "stringx.h"
#include <stdint.h>

void *fslc_memset_l(void *ptr, unsigned long value, size_t num)
{
    unsigned char *b_src = (unsigned char *)&value;
    unsigned char *b_dst = (unsigned char *)ptr;

    /* Byte by byte - to aligned word address */
    b_src += ((uintptr_t)b_dst & (sizeof(unsigned long)-1));
    for (;((uintptr_t)b_dst & (sizeof(unsigned long)-1)) && num > 0; b_dst++,b_src++,num--)
    {
        *b_dst = *b_src;
    }

    /* All aligned */
    unsigned long *w_dst = (unsigned long *)b_dst;
    for (;num >= sizeof(unsigned long); w_dst++,num -= sizeof(unsigned long))
    {
        *w_dst= value;
    }

    /* Unaligned ending */
    b_src = (unsigned char *)&value;
    b_dst = (unsigned char *)w_dst;
    for (; num > 0; b_dst++,b_src++,num--)
    {
        *b_dst = *b_src;
    }
    return ptr;
}
