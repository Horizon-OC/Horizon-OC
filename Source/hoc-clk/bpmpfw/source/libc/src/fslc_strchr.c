#include "fslc_string.h"

char *fslc_strchr(const char *str, int c)
{
    for (;*str; ++str)
        if (*str == c)
            return (char *)str;

    return NULL;
}
