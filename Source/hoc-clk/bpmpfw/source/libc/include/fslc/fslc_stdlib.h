#ifndef FSLC_STDLIB_H
#define FSLC_STDLIB_H

#include <stddef.h>

#ifndef ALT_FSLC_NAMES

#define fslc_bsearch     bsearch

#endif /* ALT_FSLC_NAMES */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


    void *fslc_bsearch(const void *key, const void *base, size_t num, size_t size, int (*cmp_proc)(const void *, const void *));


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FSLC_STDLIB_H */
