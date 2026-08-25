#include "fslc_stdio.h"

int _fslc_fputs_impl(const char *str, FSLC_FILE *stream);

int fslc_fputs(const char *str, FSLC_FILE *stream)
{
    if (stream->pre_output) stream->pre_output(stream);
    
    int r = _fslc_fputs_impl(str, stream);
    
    if (stream->post_output) stream->post_output(stream);
    
    return r;
}

