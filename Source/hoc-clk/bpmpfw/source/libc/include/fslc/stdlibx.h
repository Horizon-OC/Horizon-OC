#ifndef FSLC_STDLIBX_H
#define FSLC_STDLIBX_H

#include <stddef.h>

#ifndef ALT_FSLC_NAMES

#define fslc_bsearch_i     bsearch_i

#endif /* ALT_FSLC_NAMES */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    int fslc_bsearch_i(const void *key, const void *base, size_t num, size_t size, int (*cmp_proc)(const void *, const void *));

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FSLC_STDLIBX_H */
