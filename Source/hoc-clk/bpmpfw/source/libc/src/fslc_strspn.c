#include "fslc_string.h"

size_t fslc_strspn(const char *str, const char *delim)
{
    size_t r;

    for (r = 0; *str; ++str, ++r)
    {
        const char *d;
        for (d = delim; *d; ++d)
            if (*str == *d) // skip to next if found
                break;

        if (*d == 0) // inner loop exited on \0 - char was not there
            break;
    }
    return r;
}
