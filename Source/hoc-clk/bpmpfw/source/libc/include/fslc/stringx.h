#ifndef FSLC_STRINGX_H
#define FSLC_STRINGX_H

#include <stddef.h>

#ifndef ALT_FSLC_NAMES

#define fslc_memset_l   memset_l
#define fslc_strcpy_e   strcpy_e
#define fslc_strncpy_e  strncpy_e

#endif /* ALT_FSLC_NAMES */

#if __SIZEOF_LONG__ == 4
#define memset_32    memset_l
#elif __SIZEOF_LONG__ == 8
#define memset_32(A, V, S)	memset_l(A, (((unsigned long)(V)<<32)|(V)), S)
#endif


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    void *fslc_memset_l(void *ptr, unsigned long value, size_t num);
    char *fslc_strcpy_e(char *dest, const char *src);
    char *fslc_strncpy_e(char *dest, const char *src, size_t len);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FSLC_STRINGX_H */
