/* putchar.c */

#include <stdio.h>

int putchar(int c)
{
    return putc(CONSOLE, c);
}
