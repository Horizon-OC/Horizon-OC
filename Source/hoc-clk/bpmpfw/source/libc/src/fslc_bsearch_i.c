#include <stddef.h>

int fslc_bsearch_i(const void *key, const void *base, size_t num, size_t size, int (*cmp_proc)(const void *, const void *))
{
    int lo = 0;
    int hi = num - 1;

    char *cbase = (char *)base;

    while (lo <= hi)
    {
        int i = lo + ((hi - lo) >> 1);

        char *ptr = cbase + (i * size);
        int cmp = cmp_proc(key, ptr);

        if (cmp == 0)
            return i;

        if (cmp > 0)
        {
            lo = i + 1;
        }
        else
        {
            hi = i - 1;
        }
    }

    return ~lo;
}
