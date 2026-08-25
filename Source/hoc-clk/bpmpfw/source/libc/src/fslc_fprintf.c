#include "fslc_stdio.h"

int fslc_fprintf(FSLC_FILE *stream, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int r = fslc_vfprintf(stream, format, args);
    va_end(args);
    return r;
}
