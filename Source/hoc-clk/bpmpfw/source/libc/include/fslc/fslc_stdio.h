#ifndef FSLC_STDIO_H
#define FSLC_STDIO_H

#include <stdarg.h>
#include <stddef.h>

#ifndef ALT_FSLC_NAMES

#define FSLC_FILE       FILE
#define fslc_stdout     stdout
#define fslc_stdin      stdin
#define fslc_putchar    putchar
#define fslc_fputc      fputc
#define fslc_fputs      fputs
#define fslc_puts       puts

#define fslc_vfprintf   vfprintf
#define fslc_vprintf    vprintf
#define fslc_fprintf    fprintf
#define fslc_printf     printf

#define fslc_fwrite     fwrite
#define fslc_fread      fread

#define fslc_getc       getc
#define fslc_getchar    getchar
#define fslc_ungetc     ungetc
#define fslc_fgets      fgets

#endif /* ALT_FSLC_NAMES */

typedef struct _FSLC_FILE FSLC_FILE;

struct _FSLC_FILE
{
    void *user_ptr;
    int (*putc)(int c, FSLC_FILE *stream);
    int (*getc)(FSLC_FILE *stream);
    void (*pre_output)(FSLC_FILE *stream);
    void (*post_output)(FSLC_FILE *stream);
    int ungetc_buf;
};

extern FSLC_FILE    *fslc_stdout;
extern FSLC_FILE    *fslc_stdin;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    int fslc_putchar(int c);
    int fslc_fputc(int c, FSLC_FILE *stream);
    int fslc_fputs(const char *str, FSLC_FILE *stream);
    int fslc_puts(const char *str);

    int fslc_vfprintf(FSLC_FILE *stream, const char *format, va_list arg);
    int fslc_vprintf(const char *format, va_list arg);
    int fslc_fprintf(FSLC_FILE *stream, const char *format, ...);
    int fslc_printf(const char *format, ...);

    size_t fslc_fwrite(const void *ptr, size_t size, size_t count, FSLC_FILE *stream);
    size_t fslc_fread(void *ptr, size_t size, size_t count, FSLC_FILE *stream);

    int fslc_getc(FSLC_FILE *stream);
    int fslc_getchar(void);
    int fslc_ungetc(int c, FSLC_FILE *stream);
    char *fslc_fgets(char *str, int num, FSLC_FILE *stream);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif /* FSLC_STDIO_H */
