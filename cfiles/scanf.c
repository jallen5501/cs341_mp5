/* scanf.c - scanf,  getch, ungetch */

/* Character look-ahead is implemented with a local one-char buffer
   rather than with Xinu read-ahead (ch=control(unit,TCNEXTC))
   so that the same library can be used standalone as with Xinu.
   This only affects code that switches from scanf to getc,
   or scanf to scanf, leaving a scanf-terminating char in the
   buffer that isn't ever used again.  It's usually newline or other
   white space that gets skipped over anyway. */

/* Oct 2013: simplified to just scanf, since new Xinu lib has
   the others */

#include <sysapi.h>
#include <ctype.h>

static int getch(int dev, int *buf);
static int ungetch(int dev, int *buf);
extern int _doscan(char *fmt, int **argp, int (*getch)(int, int *),
		   int (*ungetch)(int, int *), int arg1, int arg2 );

#define	EMPTY	(-2)
#define EMPTYFLAG (1<<12)	

/*-----------------------------------------------------------------------
 *  scanf  --  read from the console according to a format
 *------------------------------------------------------------------------
 */
int scanf(char *fmt, int args)
{
  int	buf;			/* for one-char buffer */

  buf = EMPTYFLAG;
  return(_doscan(fmt, (int **)&args, getch, ungetch, CONSOLE, (int)&buf));
}


/*------------------------------------------------------------------------
 *  getch  --  get a character from a device with pushback
 *------------------------------------------------------------------------
 */
static int getch(int dev, int *buf)
{
        int ch;

	if( *buf&EMPTYFLAG)
		*buf = getc(dev)&0x7f; /* make sure one there */
	ch = *buf;		/* pick up buffered char */
	*buf |= EMPTYFLAG;		/* none there now */
	return(ch);
}

/*------------------------------------------------------------------------
 *  ungetch  --  pushback a character for getch
 *------------------------------------------------------------------------
 */
static int ungetch(int dev, int *buf)
{
	*buf &= (~EMPTYFLAG);	/* turn off emptyflag */
	return 0;
}

