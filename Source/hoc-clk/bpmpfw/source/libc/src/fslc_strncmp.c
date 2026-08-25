#include "fslc_string.h"

int fslc_strncmp(const char *str1, const char *str2, size_t num)
{
    for( ; *str1 && *str1 == *str2 && num > 1; ++str1, ++str2, --num);

    return *((unsigned char *)str1) - *((unsigned char *)str2);
}
