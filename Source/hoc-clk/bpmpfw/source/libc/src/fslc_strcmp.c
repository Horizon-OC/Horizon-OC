#include "fslc_string.h"

int fslc_strcmp(const char *str1, const char *str2)
{
    for( ; *str1 && *str1 == *str2; ++str1, ++str2);

    return *((unsigned char *)str1) - *((unsigned char *)str2);
}
