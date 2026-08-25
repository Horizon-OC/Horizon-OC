#ifndef FSLC_STRING_H
#define FSLC_STRING_H

#include <stddef.h>

#ifndef ALT_FSLC_NAMES
#define fslc_memset     memset
#define fslc_memcpy     memcpy
#define fslc_memmove    memmove
#define fslc_memcmp     memcmp

#define fslc_strlen     strlen
#define fslc_strcpy     strcpy
#define fslc_strncpy    strncpy
#define fslc_strcmp     strcmp
#define fslc_strncmp    strncmp
#define fslc_strchr     strchr
#define fslc_strstr     strstr
#define fslc_strpbrk    strpbrk
#define fslc_strspn     strspn
#define fslc_strtok_r   strtok_r

#endif /* ALT_FSLC_NAMES */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    void *fslc_memset(void *ptr, int value, size_t num);
    void *fslc_memcpy(void *dest, const void *src, size_t len);
    void *fslc_memmove(void *dest, const void *src, size_t len);
    int fslc_memcmp(const void *ptr1, const void *ptr2, size_t num);

    size_t fslc_strlen(const char *str);
    char *fslc_strcpy(char *dest, const char *src);
    char *fslc_strncpy(char *dest, const char *src, size_t len);
    int fslc_strcmp(const char *str1, const char *str2);
    int fslc_strncmp(const char *str1, const char *str2, size_t num);
    char *fslc_strchr(const char *str, int c);
    char *fslc_strstr(const char *search_in, const char *search_for);
    char *fslc_strpbrk(const char *str, const char *delim);
    size_t fslc_strspn(const char *str, const char *delim);
    char *fslc_strtok_r(char *str, const char *delim, char **save_p);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif /* FSLC_STRING_H */
