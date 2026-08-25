#include "fslc_string.h"
#include <stdint.h>

int fslc_memcmp(const void *ptr1, const void *ptr2, size_t num)
{
    unsigned char *b_p1 = (unsigned char *)ptr1;
    unsigned char *b_p2 = (unsigned char *)ptr2;

    for (;((uintptr_t)b_p1 & (sizeof(long)-1)) && num > 0; ++b_p1, ++b_p2, --num)
    {
        if (*b_p1 != *b_p2)
        {
            return *b_p1 > *b_p2 ? 1 : -1;
        }
    }

    unsigned long *w_p1 = (unsigned long *)b_p1;
    unsigned long *w_p2 = (unsigned long *)b_p2;
    for (;num >= sizeof(unsigned long); ++w_p1, ++w_p2, num -= sizeof(unsigned long))
    {
        if (*w_p1 != *w_p2)
        {
            return *w_p1 > *w_p2 ? 1 : -1;
        }
    }

    b_p1 = (unsigned char *)w_p1;
    b_p2 = (unsigned char *)w_p2;
    for (; num > 0; ++b_p1, ++b_p2, --num)
    {
        if (*b_p1 != *b_p2)
        {
            return *b_p1 > *b_p2 ? 1 : -1;
        }
    }

    return 0;
}
