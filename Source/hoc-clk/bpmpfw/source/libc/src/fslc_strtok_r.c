#include "fslc_string.h"

char *fslc_strtok_r(char *str, const char *delim, char **save_p)
{
    if (str)
        *save_p = str;

    if (*save_p == NULL)
        return NULL;

    char *r = *save_p + fslc_strspn(*save_p, delim);

    char *p = fslc_strpbrk(r, delim);

    if (p)
    {
        *(p++) = 0;
        *save_p = p;
    }
    else
    {
        if (*r == 0) r = NULL;
        *save_p = NULL;
    }

    return r;
}
