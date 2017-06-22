/* kprintf.c - printf running with ints off, for debugging */
#include <sysapi.h>

#define	OK	1
/*------------------------------------------------------------------------
 *  printf  --  write formatted output on console 
 *------------------------------------------------------------------------
 */
kprintf(fmt, args)
	char	*fmt;
{
        int saved_eflags;

        saved_eflags = get_eflags();
	cli();
	_doprnt(fmt, &args, putc, CONSOLE);
	set_eflags(saved_eflags);
	return(OK);
}
