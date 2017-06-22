/******************************************************************
*
*   file:     cmds.c
*   bob wilson: modified for mp3 on tutor VM 5/24/2016
*   bob wilson: changed the COM1 driver part of the assignment - 2/17/2008
*   bob wilson:  added solution for mp5 - 11/27/2004
*   author:   betty o'neil
*   date:     dim dark past
*/

/* the Makefile arranges that #include <..> searches in the right
   places for these headers-- */
#include <stdio.h>
#include "slex.h"
#include <cpu.h>
#include "tickpack.h"
#include <serial.h>
#include "comintspack.h"

#define PROMPT "Prompt: \n\r"
/*===================================================================*
*
*   Command table for tutor program - an array of structures of type cmd
*   - for each command provide the token, the function to call when 
*   that token is found, and the help message.
*
*   slex.h contains the typdef for struct cmd, and declares the 
*   cmds array as extern to all the other parts of the program.
*   Code in slex.c parses user input command line and calls the
*   requested semantic action, passing a pointer to the cmd struct
*   and any arguments the user may have entered.
*
*===================================================================*/
#define USECS_PER_SEC 1000000
#define MAXLINE 200

PROTOTYPE int quit(Cmd *cp, char *arguments);
PROTOTYPE int mem_display(Cmd *cp, char *arguments);
PROTOTYPE int mem_set(Cmd *cp, char *arguments);
PROTOTYPE int help(Cmd *cp, char *arguments);
PROTOTYPE int timeon ( Cmd *cp, char *arguments);
PROTOTYPE int timeoff ( Cmd *cp, char *arguments);
PROTOTYPE int spi ( Cmd *cp, char *arguments);
PROTOTYPE void process_input(char *buffer);
PROTOTYPE void process_output(char *buffer);
PROTOTYPE void tick_print(void);

/* command table */

Cmd cmds[] = {{"timeon", timeon, "Timer On: timeon <interval in secs>"},
             {"timeoff", timeoff, "Timer Off: timeoff "},
             {"spi", spi, "Serial Port Interrupts: spi <on/off>"},
             {"md",  mem_display, "Memory display: md <addr>"},
             {"ms", mem_set, "Memory set: ms <addr>"},
             {"h", help, "Help: h <cmd> or h (for all)"},
             {"q",  quit, "Quit" },
             {NULL, NULL,  NULL}};   /* null cmd to flag end of table */

/*===================================================================*
*		command			routines		     
*
*   Each command routine is called with 2 args, the remaining 
*   part of the line to parse and a pointer to the struct cmd for this 
*   command. Each returns 0 for continue or 1 for all-done.  
*
*===================================================================*/

static int timer_on;   		/* flag for timer int on */
static int ticks;               /* tick count to display */

static int port_on;		/* port ints are enabled */
static char buffer[MAXLINE];
/*===================================================================
*
*     timeon: timer interrupts every interval seconds and displays count
*
*====================================================================*/

int timeon (Cmd *cp, char *arguments)
{
  unsigned int interval;

  if ((sscanf(arguments, "%d", &interval))!=1) {
    printf ("Proper usage : %s\n", cp->help);
    return 0; 
  }
  
  if(timer_on) {
    printf("%s", "timer already on\n");
    return 0;
  }
  set_timer(&tick_print, interval, interval);   /* continuous */  
  timer_on = 1;
  ticks = 0;
  printf("%s", "timer on\n");
  return 0;
}

void tick_print(void)
{
  ticks++;
  printf("(%d)\n", ticks);
}

/*=====================================================================
*
*     timeoff: shut down timer and stop printing of count
*
*======================================================================*/

int timeoff (Cmd *cp, char *arguments)
{
  stop_timer(&tick_print);
  timer_on = 0;
  ticks = 0;
  printf("%s", "timer off\n");
  return 0;
}

/*=====================================================================
*
*     spi: set serial port interrupts either on or off
*
*======================================================================*/

int spi (Cmd *cp, char *arguments)
{
   char on_off[MAXLINE];

   if (sscanf(arguments,"%s", on_off) != 1){
     printf ("Proper Usage : %s\n", cp->help);
     return 0; 
   }

   if(!strcmp(on_off,"on")) {   /* turning on */ 
     port_on = 1;
     /* send prompt to user on COM1 then alternately 
      * print user entries to COM2 and prompts to COM1 
      * via interrupts and the callback functions below 
      */

     cli();       /* see comments in comintpack.c */
     init_comints(TRANSMIT, process_output, PROMPT, strlen(PROMPT));
     sti();
     printf("comints for COM1 on\n");
   } 
   else {                       /* turning off */
     cli();      /* see comments in comintspack.c */ 
     shutdown_comints();
     sti();
     port_on = 0;
     printf("comints for COM1 off\n");
   }
   return 0;
}

/* interupts are disabled during execution of these two callback functions */

void process_output(char *prompt)
{
     /* you write the code here */


}

void process_input(char *received)
{
     /* you write the code here */


}
/*===================================================================*
*		command			routines		     
*
*   Each command routine is called with 2 args, the remaining 
*   part of the line to parse and a pointer to the struct cmd for this 
*   command. Each returns 0 for continue or 1 for all-done.  
*
*===================================================================*/

int quit(Cmd *cp, char *arguments)
{
  if (timer_on) {
     shutdown_ticks();
     timer_on = 0;
  }
  if (port_on) {
     cli();      /* see comments in comintspack.c */ 
     shutdown_comints();
     sti();
     port_on = 0;
  }
  return 1;			/* all done flag */
}

/*===================================================================*
*
*   mem_display: display contents of a single byte in hex
*
*/

int mem_display(Cmd *cp, char *arguments)
{
  unsigned int address, i;

  if (sscanf(arguments, "%x", &address) == 1) {
    /* print the address in hex */
    printf("%08x    ", address);
    /* print 16 locations as two hex digits per memory byte */ 
    for (i = 0; i < 16; i++)
      printf("%02x ", *(unsigned char *)(address + i));
    /* print 16 locations as one ascii coded character per byte */
    for (i = 0; i < 16; i++)
      printf("%c",
      (*(unsigned char *)(address + i) >= ' ' &&
       *(unsigned char *)(address + i) <= '~') ?
       *(unsigned char *)(address + i) : '.');
    /* and print end of line */
    printf("\n");
  } else {
    printf("%s\n", cp->help);
  }
  return 0;
}

/********************************************************/

/* mem_set : ms <hexaddress> <new_val> 
 *     stores byte new_val at address, both are given 
 *     in 'arguments'
*/

int mem_set(Cmd *cp, char *arguments) 
{
  unsigned int address, value;

  if (sscanf(arguments, "%x %x", &address, &value) == 2) {
    if(value < 0x100)
       *(unsigned char *)address = value;
    else
       *(unsigned int *)address = value;

    printf("OK\n");
  } else {
    printf("%s\n", cp->help);
  }
  return 0;
}

/****************************************************/

/* help : display a help message of the 'arguments'
 *      command, or list all commands if no arg.
*/

int help(Cmd *cp, char *arguments) {
  int stat, cmd_num, pos;

  /* reuse code that matches command token-- */
  stat = slex(arguments, cmds, &cmd_num, &pos);
  if (stat < 0) {		/* if no command token found in args */
    printf("     cmd    help message\n");
    printf("     ---    ------------\n");
    for (cp = cmds; cp->cmdtoken; cp++)   /* show all commands */
      printf("%8s    %s\n",cp->cmdtoken, cp->help);
  } else {			/* found command token, just print that one */
    printf("%s\n", cmds[cmd_num].help);
    return 0;
  }
  return 0;
}
