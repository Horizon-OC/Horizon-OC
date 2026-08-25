#include "fslc_stdio.h"

int fslc_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int r = fslc_vfprintf(fslc_stdout, format, args);
    va_end(args);
    return r;
}
