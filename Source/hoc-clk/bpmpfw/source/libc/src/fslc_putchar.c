#include "fslc_stdio.h"

int fslc_putchar(int c)
{
    return fslc_fputc(c, fslc_stdout);
}
