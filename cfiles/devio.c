/* devio.c --support for device indep routines, */
/* and invents CONSOLE pseudodevice: console i/o, whereever console is */
/* by Betty O'Neil, Dec., 1995 */

/* init, putc, getc, readyc for each device.  The downloaded
 * program can use these for basic i/o services, as well as 
 * the kernel (each of which uses a copy of this file in builds)
 */
#include <stdio.h>

/* for downloaded program only--latch onto Tutor-supplied code */
#ifndef SAPC_KERNEL  
#include <params.h>

SysAPI2 *sysapi2 = 0;		/* master pointer to Tutor dispatch table */

void init_devio()
{
#ifdef NEED_SCREEN_INFO
  screen_info = SCREEN_INFO;
#endif
  sysapi2 = SYS_API;	/* establish master pointer */
}
#endif

/* initialize device drivers--normally only used by kernel */
unsigned long init(int dev, unsigned long kmem_start)
{
  if (dev < 0 || dev >= MAXSYSDEVS || !sys_devname(dev)[0])
    return kmem_start;
  return sys_init(dev, kmem_start);
}

/* output one char, by polling or equivalent, no interpretation
 * of char, but dev CONSOLE->console_dev mapping provided */
int rawputc(int dev, char ch)
{
  if (dev == CONSOLE)
    dev = sys_get_console_dev();
  if (dev < 0 || dev >= MAXSYSDEVS || !sys_devname(dev)[0])
    return -1;
  return sys_putc(dev, ch);
}

#ifndef SAPC_KERNEL  
/* a msec or so */
static void delay()
{
  int i;

  for (i=0;i<20000;i++)
    ;
}
#endif

/* output one char, with lf-> crlf, CONSOLE->console_dev, 
 * broadcast if dev < 0, do debug protocol if live debugline */
int putc(int dev, char ch)
{
  if (dev >= MAXSYSDEVS)
    return -1;
  if ((dev == CONSOLE) && (sys_get_console_dev()<0))
    dev = -1;			/* initial broadcast */
  if (dev < 0) {		/* broadcast */
    int i;

    for (i=0;i<MAXDEVS;i++)
      putc(i, ch);		/* real devs */
    return 0;
  }
  /* non-broadcast-- */
  if (ch == 0)
    return 0;
  if (dev == CONSOLE) {
    int debug_dev;

    dev = sys_get_console_dev();
    if ((debug_dev = sys_get_debugline_dev())>=0) {
      rawputc(DEBUG_CONSOLE,ch); /* output to debug console too */
      if (dev == debug_dev)	/* if console is debugline */
	return 0;		/* only do debug protocol  */
    }
  }
#ifndef SAPC_KERNEL  
  if (dev == sys_get_hostline_dev())
    delay();			/* be nice to host */
#endif
  if (ch == '\n') {
    rawputc(dev, '\r');
    rawputc(dev, '\n');
  } else {
    rawputc(dev, ch);
  }
  return 0;
}

/* get one char from device by polling or equiv., no interp. of
 * char, but CONSOLE mapping provided */
int rawgetc(int dev)
{
  if (dev == CONSOLE)
    dev = sys_get_console_dev();
  if (dev < 0 || dev >= MAXSYSDEVS || !sys_devname(dev)[0])
    return -1;
  return sys_getc(dev);
}

/* get one char from device by polling, echo it for CONSOLE,
   convert CR to newline, abort if requested */
int getc(int dev)
{
  int c = rawgetc(dev);

  if (c<0)
    return -1;
  if (c==EOFCHAR)
    return EOF;
  if (c=='\r')			/* user CR -> '\n' */
    c = '\n';
  if (c==ESCAPE_TO_TUTOR)
    breakpoint();		/* special escape to Tutor */
  if (dev==CONSOLE)
    putc(dev, c);		/* echo for CONSOLE */
  return c;
}


/* check if char ready to be getc'd (returns Boolean) */
int readyc(int dev)
{
  if (dev == CONSOLE)
    dev = sys_get_console_dev();
  if (dev < 0 || dev >= MAXSYSDEVS || !sys_devname(dev)[0])
    return -1;
  return sys_readyc(dev);
}

int devcontrol(int dev, int fn, void * param)
{
  if (dev == CONSOLE)
    dev = sys_get_console_dev();
  if (dev < 0 || dev >= MAXSYSDEVS || !sys_devname(dev)[0])
    return -1;
  return sys_devcontrol(dev, fn, param);
}

int devdescript(int dev, char *descript)
{  
  if (dev == CONSOLE)
    dev = sys_get_console_dev();
  if (dev < 0 || dev >= MAXSYSDEVS || !sys_devname(dev)[0])
    return -1;
  return sys_devdescript(dev, descript);
}
   
char *devname(int dev)
{
  if (dev == CONSOLE)
    dev = sys_get_console_dev();
  if (dev < 0 || dev >= MAXSYSDEVS || !sys_devname(dev)[0])
    return 0;
  return sys_devname(dev);
}
  
