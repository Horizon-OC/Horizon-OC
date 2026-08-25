#include "fslc_stdio.h"

int fslc_vprintf(const char *format, va_list arg)
{
    return fslc_vfprintf(fslc_stdout, format, arg);
}
