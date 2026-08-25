#include "fslc_stdio.h"

int fslc_fputc(int c, FSLC_FILE *stream)
{
    if (stream->pre_output) stream->pre_output(stream);
    
    int r = stream->putc(c, stream);
    
    if (stream->post_output) stream->post_output(stream);
    
    return r;
}
