#include "fslc_string.h"

char *fslc_strstr(const char *search_in, const char *search_for)
{
    while (*search_in)
    {
        char *rc = fslc_strchr(search_in, search_for[0]); // find first char

        if (rc)
        {
            const char *si = rc + 1, *sf = search_for + 1; // search strcmp-style from second char
            for( ; *si && *si == *sf; ++si, ++sf);

            if (*sf == 0) return rc; // end of search-for - found result

            search_in = rc + 1; // not this one, continue from next char
        }
        else
            return NULL; // did not find first char - exit
    }

    return NULL; // reached end of source - did not find
}
