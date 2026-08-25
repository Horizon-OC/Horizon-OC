#include "fslc_stdio.h"

int _fslc_fputs_impl(const char *str, FSLC_FILE *stream);

int fslc_puts(const char *str)
{
    if (fslc_stdout->pre_output) fslc_stdout->pre_output(fslc_stdout);
    
    int r = _fslc_fputs_impl(str, fslc_stdout);
    
    if (r >= 0)
    {
        int nlr = fslc_stdout->putc('\n', fslc_stdout);
        if (nlr < 0)
            r = nlr;
    }
    
    if (fslc_stdout->post_output) fslc_stdout->post_output(fslc_stdout);
    
    return r;
}
