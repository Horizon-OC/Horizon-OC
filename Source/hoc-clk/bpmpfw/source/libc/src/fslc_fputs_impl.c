#include "fslc_stdio.h"

int _fslc_fputs_impl(const char *str, FSLC_FILE *stream)
{
    int res = 0;
    for (;*str; ++str)
    {
        int pr = stream->putc(*str, stream);
        if (pr < 0) return pr;
        ++res;
    }
    return res;
}
