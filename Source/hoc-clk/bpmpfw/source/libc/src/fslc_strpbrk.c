#include "fslc_string.h"

char *fslc_strpbrk(const char *str, const char *delim)
{
    for (; *str; ++str)
    {
        const char *d;
        for (d = delim; *d; ++d)
            if (*str == *d)
                return (char *)str;
    }

    return NULL;
}
