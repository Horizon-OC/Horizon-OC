#ifndef FSLC_ASSERT_H
#define FSLC_ASSERT_H

#ifndef ALT_FSLC_NAMES

#define fslc_assert     assert

#endif /* ALT_FSLC_NAMES */


#ifdef NDEBUG

#define fslc_assert(expr)     ((void)0)

#else /* NDEBUG */

/* Note: __PRETTY_FUNCTION__ is GCC specific */
#define fslc_assert(expr)     ((expr) ? (void)0 : __fslc_assert_fail(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__))

#endif /* NDEBUG */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


    void __fslc_assert_fail(const char *expr, const char *file, unsigned int line, const char *func);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FSLC_ASSERT_H */
