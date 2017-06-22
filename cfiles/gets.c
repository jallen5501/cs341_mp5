/* gets.c - gets */
#include <stdio.h>

/*------------------------------------------------------------------------
 *  gets  -- gets string from the console device reading to user buffer
 *------------------------------------------------------------------------
 */
char *gets(s)
        char *s;
{
	register c;
	register char *cs;

	cs = s;
        while ((c = getc(CONSOLE)) != '\n' && c != '\r' && c != EOFCHAR)
		*cs++ = c;
	if (c==EOFCHAR && cs==s)
		return(NULL);
	*cs++ = '\0';
	return(s);
}
