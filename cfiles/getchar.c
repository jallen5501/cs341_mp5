/* getchar.c */

#include <stdio.h>

int getchar(void)
{
    return fgetc(CONSOLE);
}
