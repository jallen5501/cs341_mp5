/* printf.c */

#include <stdio.h>
#include <stdarg.h>

extern void _fdoprnt(char *, va_list, int (*)(int, char), int);

int printf(const char *fmt, ...)
{
    va_list ap;
    extern int putc(int, char);

    va_start(ap, fmt);
    _fdoprnt((char *)fmt, ap, putc, CONSOLE);
    va_end(ap);

    return 0;
}
