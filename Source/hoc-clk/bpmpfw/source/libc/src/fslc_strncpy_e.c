#include "stringx.h"

char *_fslc_strncpy_impl(char *dest, const char *src, size_t len);

char *fslc_strncpy_e(char *dest, const char *src, size_t len)
{
    char *r = _fslc_strncpy_impl(dest, src, len);
    return *r ? r + 1: r;
}
