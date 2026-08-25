#include "fslc_string.h"

char *_fslc_strncpy_impl(char *dest, const char *src, size_t len);

char *fslc_strncpy(char *dest, const char *src, size_t len)
{
    _fslc_strncpy_impl(dest, src, len);
    return dest;
}
