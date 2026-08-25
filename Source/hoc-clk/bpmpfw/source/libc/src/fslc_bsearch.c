#include "fslc_stdlib.h"
#include "stdlibx.h"

void *fslc_bsearch(const void *key, const void *base, size_t num, size_t size, int (*cmp_proc)(const void *, const void *))
{
    int sidx = fslc_bsearch_i(key, base, num, size, cmp_proc);

    if (sidx < 0)
        return NULL;

    return (char *)base + (sidx * size);
}
