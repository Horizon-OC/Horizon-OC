#include "fslc_string.h"
#include "stringx.h"

void *fslc_memset(void *ptr, int value, size_t num)
{
    /* Will use memset_l() to actually fill the memory region. If value is non-zero, we need
     * to replicate lowest byte to all the long's bytes.
     */
    if (value)
    {
        unsigned char c_val = value;
        unsigned long l_val = (c_val << 0) | (c_val << 8) | (c_val << 16) | (c_val << 24);

        #if __SIZEOF_LONG__ == 8
        l_val |= l_val << 32;
        #endif

        return fslc_memset_l(ptr, l_val, num);
    }
    else
        return fslc_memset_l(ptr, value, num);
}
