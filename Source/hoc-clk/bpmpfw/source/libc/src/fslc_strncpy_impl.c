#include <stddef.h>

char *_fslc_strncpy_impl(char *dest, const char *src, size_t len)
{
    for(; *src && len > 1; ++src, ++dest, --len)
        *dest = *src;

    *dest = *src;
    return dest;
}
