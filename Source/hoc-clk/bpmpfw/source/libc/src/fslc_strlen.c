#include "fslc_string.h"

size_t fslc_strlen(const char *str)
{
    size_t res = 0;
    for(; *str; ++str) ++res;

    return res;
}
