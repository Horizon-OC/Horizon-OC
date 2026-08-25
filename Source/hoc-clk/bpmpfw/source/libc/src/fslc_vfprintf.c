#include "fslc_stdio.h"

/* There'a a funny behaviour of va_list and va_arg() macros:
 *    - on x86_64 compiler generates a branch of some sort for each va_arg() macro. Normally it is not a problem,
 *      however it kinda screws up branch coverage analysis. Therefore I decided to move va_arg() accesses into
 *      dedicated functions, minimizing the issue to one untaken branch per access function.
 *    - passing va_list by value works fine on x86_64, but makes some tests to fail on i386. It seems that modifications
 *      done by va_arg() is not passed back to caller. Subsequent calls to va_arg() or access functions returns wrong
 *      values on that platform.
 *    - OK, then. Let's pass pointer to va_list instead. Not so fast. Works fine on i386, but fails to compile on
 *      x86_64. Fine! Wrapping va_list into a dedicated struct and passing around pointer to it.
 *
 *    Why such a fuss about a few untaken branches in tests? Well, just because! Also, if I ever decide to refactor
 *    _fslc_vfprintf_impl() - the case statement is getting a bit long and messy, it should make things easier if
 *    argument list can be passed around freely (as opposed to using it from single function).
 */

struct va_list_w
{
    va_list arg;
};

/* Prototype for printf's internal implementation */
static int _fslc_vfprintf_impl(FSLC_FILE *stream, const char *format, struct va_list_w *args);


/* Main exported function */
int fslc_vfprintf(FSLC_FILE *stream, const char *format, va_list arg)
{
    if (stream->pre_output) stream->pre_output(stream);

    struct va_list_w argw;
    va_copy(argw.arg, arg); // can not assign directly, should make a copy

    int res = _fslc_vfprintf_impl(stream, format, &argw);

    va_end(argw.arg); // and free it after use

    if (stream->post_output) stream->post_output(stream);

    return res;
}

/* Prototype to private FSLIBC function - implemented elsewhere */
int _fslc_fputs_impl(const char *str, FSLC_FILE *stream);


/* Prototypes for internal functions */
static int _fslc_put_sint_l(signed long v, FSLC_FILE *stream);
static int _fslc_put_uint_l(unsigned long v, FSLC_FILE *stream);
static int _fslc_put_hex_l(unsigned long v, FSLC_FILE *stream, char alpha);

static int _get_sint_arg(struct va_list_w *arg);
static unsigned _get_uint_arg(struct va_list_w *arg);
static long long _get_slonglong_arg(struct va_list_w *arg);
static unsigned long long _get_ulonglong_arg(struct va_list_w *arg);
static void *_get_ptr_arg(struct va_list_w *arg);

/* There are some considerations when formatting Integers for output:
 *     - on 32-bit systems integers will mostly be 32 bits long, except when one REALLY
 *       wants to print long long. It should be more optimal to use 32-bit calculations
 *       when possible and invoke 64-bit only when needed. To support that we provide 2
 *       separate implementations.
 *     - on 64-bit systems may be 32 or 64 bits, but there is no real benefit from
 *       using shorter bit size. Hence common implementation.
 *     - this library does not aim to support systems with shorter integer sizes (like 16-bit)
 *       but it it did, it would probably make sense to provide optimized ports for these cases
 *       as well.
 * 
 * Buffer size calculations:
 *   Max UInt32 fits in 10 bytes (+1 for zero terminator) decimal
 *   2**32 - 1 == 4294967295z
 *                FFFFFFFFz
 *                01234567890
 * 
 *   Max UInt64 fits in 10 bytes (and +1 for \0)
 *   2**64 -1  == 18446744073709551615z
 *                FFFFFFFFFFFFFFFFz
 *                012345678901234567890
 */

#define BUFSIZE_LONG_LONG_DECIMAL   20
#define BUFSIZE_LONG_HEX            (__SIZEOF_LONG__ * 2)
#define BUFSIZE_LONG_LONG_HEX       (__SIZEOF_LONG_LONG__ * 2)
#if __SIZEOF_LONG__ == 4

#define BUFSIZE_LONG_DECIMAL    10
static int _fslc_put_sint_ll(signed long long v, FSLC_FILE *stream);
static int _fslc_put_uint_ll(unsigned long long v, FSLC_FILE *stream);
static int _fslc_put_hex_ll(unsigned long long v, FSLC_FILE *stream, char alpha);

#define _get_slong_arg      _get_sint_arg
#define _get_ulong_arg      _get_uint_arg

#elif __SIZEOF_LONG__ == 8

#define BUFSIZE_LONG_DECIMAL    BUFSIZE_LONG_LONG_DECIMAL
#define _fslc_put_sint_ll   _fslc_put_sint_l
#define _fslc_put_uint_ll   _fslc_put_uint_l
#define _fslc_put_hex_ll    _fslc_put_hex_l

#define _get_slong_arg      _get_slonglong_arg
#define _get_ulong_arg      _get_ulonglong_arg

#endif

#define FLAG_LONG       1
#define FLAG_VERYLONG   3

static int _fslc_vfprintf_impl(FSLC_FILE *stream, const char *format, struct va_list_w *arg)
{
    int res = 0;
    int pr;
    const char *c;
    
    for (c = format; *c; ++c)
    {
        if (*c == '%')
        {
            int flags = 0;
            
            for (;;) 
            {
                ++c;
                switch (*c)
                {
                    case 's':
                        pr = _fslc_fputs_impl((const char *)_get_ptr_arg(arg), stream);
                        if (pr < 0) return pr;
                        res += pr;
                        break;

                    case 'c':
                        pr = stream->putc(_get_sint_arg(arg), stream);
                        if (pr < 0) return pr;
                        ++res;
                        break;
                        
                    case '%':
                        pr = stream->putc('%', stream);
                        if (pr < 0) return pr;
                        ++res;
                        break;
                    
                    case 'i':
                    case 'd':
                        if ((flags & FLAG_VERYLONG) == FLAG_VERYLONG)
                            pr = _fslc_put_sint_ll(_get_slonglong_arg(arg), stream);
                        else if (flags & FLAG_LONG)
                            pr = _fslc_put_sint_l(_get_slong_arg(arg), stream);
                        else
                            pr = _fslc_put_sint_l(_get_sint_arg(arg), stream);
                        
                        if (pr < 0) return pr;
                        res += pr;
                        break;
                    
                    case 'u':
                        if ((flags & FLAG_VERYLONG) == FLAG_VERYLONG)
                            pr = _fslc_put_uint_ll(_get_ulonglong_arg(arg), stream);
                        else if (flags & FLAG_LONG)
                            pr = _fslc_put_uint_l(_get_ulong_arg(arg), stream);
                        else
                            pr = _fslc_put_uint_l(_get_uint_arg(arg), stream);
                        
                        if (pr < 0) return pr;
                        res += pr;
                        break;
                    
                    case 'x':
                        if ((flags & FLAG_VERYLONG) == FLAG_VERYLONG)
                            pr = _fslc_put_hex_ll(_get_ulonglong_arg(arg), stream, 'a');
                        else if (flags & FLAG_LONG)
                            pr = _fslc_put_hex_l(_get_ulong_arg(arg), stream, 'a');
                        else
                            pr = _fslc_put_hex_l(_get_uint_arg(arg), stream, 'a');
                        
                        if (pr < 0) return pr;
                        res += pr;
                        break;
                    
                    case 'X':
                        if ((flags & FLAG_VERYLONG) == FLAG_VERYLONG)
                            pr = _fslc_put_hex_ll(_get_ulonglong_arg(arg), stream, 'A');
                        else if (flags & FLAG_LONG)
                            pr = _fslc_put_hex_l(_get_ulong_arg(arg), stream, 'A');
                        else
                            pr = _fslc_put_hex_l(_get_uint_arg(arg), stream, 'A');
                        
                        if (pr < 0) return pr;
                        res += pr;
                        break;
                        
                    case 'p':
                        pr = _fslc_fputs_impl("0x", stream);
                        if (pr < 0) return pr;
                        res += pr;

                        pr = _fslc_put_hex_l(_get_ulong_arg(arg), stream, 'a');

                        if (pr < 0) return pr;
                        res += pr;
                        break;

                    case 'l':
                        if (flags & FLAG_LONG) 
                            flags |= FLAG_VERYLONG;
                        else
                            flags |= FLAG_LONG;
                        continue;
                }
                break;
            }
        } else {
            pr = stream->putc(*c, stream);
            if (pr < 0) return pr;
            ++res;
        }
    }
    
    return res;
}

static int _get_sint_arg(struct va_list_w *arg)
{
    return va_arg(arg->arg, signed int);
}

static unsigned _get_uint_arg(struct va_list_w *arg)
{
    return va_arg(arg->arg, unsigned int);
}

static long long _get_slonglong_arg(struct va_list_w *arg)
{
    return va_arg(arg->arg, signed long long);
}

static unsigned long long _get_ulonglong_arg(struct va_list_w *arg)
{
    return va_arg(arg->arg, unsigned long long);
}

static void *_get_ptr_arg(struct va_list_w *arg)
{
    return va_arg(arg->arg, void *);
}

static int _fslc_put_sint_l(signed long v, FSLC_FILE *stream)
{
    if (v < 0)
    {
        int pr = stream->putc('-', stream);
        if (pr < 0) return pr;
        
        pr = _fslc_put_uint_l(-v, stream);
        if (pr < 0) return pr;
        
        return pr+1;
    } else
        return _fslc_put_uint_l(v, stream);
}

static int _fslc_put_uint_l(unsigned long v, FSLC_FILE *stream)
{
    char dbuff[BUFSIZE_LONG_DECIMAL+1], *p;

    dbuff[BUFSIZE_LONG_DECIMAL] = 0;

    for (p = dbuff + BUFSIZE_LONG_DECIMAL; v ; v /= 10)
    {
        *(--p) = (v % 10) + '0';
    }

    if (*p == 0) *(--p) = '0';

    return _fslc_fputs_impl(p, stream);
}

static int _fslc_put_hex_l(unsigned long v, FSLC_FILE *stream, char alpha)
{
    char dbuff[BUFSIZE_LONG_HEX+1], *p;

    dbuff[BUFSIZE_LONG_HEX] = 0;

    for (p = dbuff + BUFSIZE_LONG_HEX; v ; v >>= 4)
    {
        char digit = (v & 0xF);
        *(--p) = digit < 10 ? digit + '0' : digit - 10 + alpha;
    }

    if (*p == 0) *(--p) = '0';

    return _fslc_fputs_impl(p, stream);
}

#if __SIZEOF_LONG__ == 4

static int _fslc_put_sint_ll(signed long long v, FSLC_FILE *stream)
{
    if (v < 0)
    {
        int pr = stream->putc('-', stream);
        if (pr < 0) return pr;
        
        pr = _fslc_put_uint_ll(-v, stream);
        if (pr < 0) return pr;
        
        return pr+1;
    } else
        return _fslc_put_uint_ll(v, stream);
}

static int _fslc_put_uint_ll(unsigned long long v, FSLC_FILE *stream)
{
    char dbuff[BUFSIZE_LONG_LONG_DECIMAL+1], *p;

    dbuff[BUFSIZE_LONG_LONG_DECIMAL] = 0;

    for (p = dbuff + BUFSIZE_LONG_LONG_DECIMAL; v ; v /= 10)
    {
        *(--p) = (v % 10) + '0';
    }

    if (*p == 0) *(--p) = '0';

    return _fslc_fputs_impl(p, stream);
}

static int _fslc_put_hex_ll(unsigned long long v, FSLC_FILE *stream, char alpha)
{
    char dbuff[BUFSIZE_LONG_LONG_HEX+1], *p;

    dbuff[BUFSIZE_LONG_LONG_HEX] = 0;

    for (p = dbuff + BUFSIZE_LONG_LONG_HEX; v ; v >>= 4)
    {
        char digit = (v & 0xF);
        *(--p) = digit < 10 ? digit + '0' : digit - 10 + alpha;
    }

    if (*p == 0) *(--p) = '0';

    return _fslc_fputs_impl(p, stream);
}

#endif
