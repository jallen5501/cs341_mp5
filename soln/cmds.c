m	******************************************************************
*
*   file:     cmds.c
*   bob wilson: modified for mp3 on tutor VM 05/24/2016
*   bob wilson: modified for mp5 on tutor VM 11/20/2016
*   author:   betty o'neil
*   date:     dim dark past
*
*/

/* the makefile arranges that #include <..> searches in the right
   places for these headers-- */
#include <stdio.h>
#include <slex.h>
#include <serial.h>
#include <lp.h>
#include "tickpack.h"
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
PROTOTYPE int test( Cmd *cp, char *arguments);
PROTOTYPE void uartTimer(void);

/* command table */

Cmd cmds[] = {{"test", test, "Test a port: test LPT1 or test COM1"},
             {"timeon", timeon, "Timer On: timeon <interval in secs>"},
             {"timeoff", timeoff, "Timer Off: timeoff"},
             {"spi", spi, "Serial Port Interrupts: spi <on/off>"},
             {"md",  mem_display, "Memory display: md <addr>"},
             {"ms", mem_set, "Memory set: ms <addr>"},
             {"h", help, "Help: h <cmd> or h (for all)"},
             {"q",  quit, "Quit" },
             {NULL, NULL,  NULL}};   /* null cmd to flag end of table */

static int timer_on;   		/* flag for timer int on */
static int ticks;               /* tick count to display */
static volatile int timeout;    /* timeout for UART receive */

static int port_on;             /* port ints are enabled */
static char buffer[MAXLINE];
/*===================================================================*
*               command                 routines
*
*   Each command routine is called with 2 args, the remaining
*   part of the line to parse and a pointer to the struct cmd for this
*   command. Each returns 0 for continue or 1 for all-done.
*
*===================================================================*/

/*===================================================================
*     
*     test: on demand diagnostic for LPT1 or COM1
*
*====================================================================*/

int test( Cmd *cp, char *arguments)  
{
  char port[20];
  int i;

  /* declarations for LPT1 testing */
  unsigned char lpc_saved, lpd_saved, lpd;

  /* declarations for COM1 testing */
  unsigned char mcr_saved, mcr, msr, c;

  if (sscanf(arguments, "%s", port) != 1) {
    printf ("Proper usage : %s\n", cp->help);
    return 0;
  }
    
  if(!strcmp("LPT1", port)) {
    lpc_saved = inpt(LPT1_BASE+LP_CNTRL);     /* save LP_CNTRL */
    lpd_saved = inpt(LPT1_BASE+LP_DATA);      /* save LP_DATA  */
    outpt(LPT1_BASE+LP_CNTRL, inpt(LPT1_BASE+LP_CNTRL) & ~LP_PDIR);

    printf("Testing normal Read/Write mode\n");
    for(i = 0; i < 0x100; i++) {
      outpt(LPT1_BASE+LP_DATA, i);
      lpd = inpt(LPT1_BASE+LP_DATA);
      if (lpd != i) {
        printf("DATA: %02x %s\n", lpd, (lpd == 0xff)? "OK" : "NO");
        break;
      }   
    }
    if (i != 0x100)
      printf("Error detected!\n");      
    else
      printf("OK\n");

    outpt(LPT1_BASE+LP_CNTRL, inpt(LPT1_BASE+LP_CNTRL) | LP_PDIR);
    printf("Testing Bi-directional mode\n");

    for(i = 0; i < 0x100; i++) {
      outpt(LPT1_BASE+LP_DATA, i);
      lpd = inpt(LPT1_BASE+LP_DATA);
      if (lpd != 0xff) {
        printf("DATA: %02x %s\n\n", lpd, (lpd == 0xff)? "OK" : "NO");
        break;
      }
    }
    if (i != 0x100)
      printf("Error detected in bi-directional mode.\n"); 
    else 
      printf("OK\n");

    outpt(LPT1_BASE+LP_CNTRL, lpc_saved);      /* restore LP_CNTRL */
    outpt(LPT1_BASE+LP_DATA, lpd_saved);       /* restore LP_DATA  */
  } 
  else if(!strcmp("COM1", port)) {
    mcr_saved = inpt(COM1_BASE+UART_MCR);      /* save COM1_MCR    */

    printf("Testing normal mode\n");
    msr = inpt(COM1_BASE+UART_MSR) & 0x0f;     /* capture current status */

    for (i = 0; i < 0x10; i++) {
      outpt(COM1_BASE+UART_MCR, 0x0f);
      if ((inpt(COM1_BASE+UART_MSR) & 0x0f) != msr) {
        break;
      }
    }

    if (i != 0x10)
      printf("Error detected\n");
    else
      printf("OK\n");

    /* set loop bit */
    msr = inpt(COM1_BASE+UART_MSR);            /* clear trans bits */

    outpt(COM1_BASE+UART_MCR, inpt(COM1_BASE+UART_MCR) | UART_MCR_LOOP);   
    printf("Testing loopback mode\n");

    mcr = inpt(COM1_BASE+UART_MCR);
    printf("MCR: %02x %s\n", mcr, (mcr == 0x1f)? "OK" : "NO");
    msr = inpt(COM1_BASE+UART_MSR);
    printf("MSR: %02x %s\n", msr, (msr == 0xf4)? "OK" : "NO");
    msr = inpt(COM1_BASE+UART_MSR);
    printf("MSR: %02x %s\n", msr, (msr == 0xf0)? "OK" : "NO");
    
    outpt(COM1_BASE+UART_MCR, inpt(COM1_BASE+UART_MCR) & 0xf0);
    mcr = inpt(COM1_BASE+UART_MCR);
    printf("MCR: %02x %s\n", mcr, (mcr == 0x10)? "OK" : "NO");
    msr = inpt(COM1_BASE+UART_MSR);
    printf("MSR: %02x %s\n", msr, (msr == 0x0f)? "OK" : "NO");
    msr = inpt(COM1_BASE+UART_MSR);
    printf("MSR: %02x %s\n\n", msr, (msr == 0x00)? "OK" : "NO");

    /* debug only -  to test timeout */
    /*outpt(COM1_BASE+UART_MCR, inpt(COM1_BASE+UART_MCR) & ~UART_MCR_LOOP);*/  

    timeout = 0;                    /* timeout has not occurred */
    set_timer(uartTimer, 5, 0);     /* set one shot timer for 5 seconds */
    c = 'd';
    outpt(COM1_BASE+UART_TX, c);
    /* wait until a character is received or a timeout occurs */
    while(!(inpt(COM1_BASE+UART_LSR) & UART_LSR_DR) && !timeout)
      ;
    stop_timer(uartTimer);          /* delete the timer */
    if (timeout)
      printf("Loop Test Fails - Timeout.\n");
    else if(inpt(COM1_BASE+UART_RX) == c)
      printf("Loop Test Data Passes.\n");
    else   
      printf("Loop Test Data Fails.\n");
    
    /* reset the loopback */
    outpt(COM1_BASE+UART_MCR, inpt(COM1_BASE+UART_MCR) & ~UART_MCR_LOOP);
    printf("Not in loopback now.\n");
    outpt(COM1_BASE+UART_MCR, inpt(COM1_BASE+UART_MCR) | 0x0b);  /* restore */
    inpt(COM1_BASE+UART_MSR);   /* clear transition bits */

    outpt(COM1_BASE+UART_MCR, mcr_saved);         /* restore COM1_MCR */
  } 
  else
    printf("\nOnly LPT1 and COM1 can be tested\n");
  return 0;
}

void uartTimer()
{
  timeout = 1;	/* timeout occurred */
}

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
   /* you add code here */
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
     cli();        /* see comments in comintspack.c */
     shutdown_comints();
     sti();
     port_on = 0;
     printf("comints for COM1 off\n");
   }
   return 0;
}

/* interupts are disabled during execution of these two callback functions 
*/

void process_output(char *prompt)
{
     /* you write the code here */
     shutdown_comints();
     /* now get the user entered data */    
     init_comints(RECEIVE, process_input, buffer, MAXLINE);
}

void process_input(char *received)
{
     /* you write the code here */
     shutdown_comints();
     /* print the data received from COM1 on COM2 */
     printf("%s\n", received);
     /* and put out the prompt again */
     init_comints(TRANSMIT, process_output, PROMPT, strlen(PROMPT));
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
  } else
    printf("        help message: %s\n", cp->help);

  return 0;                   /* not done */
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
  } else
    printf("        help message: %s\n", cp->help);

  return 0;
}

/****************************************************/

/* help : display a help message of the 'arguments'
 *      command, or list all commands if no arg.
*/

int help(Cmd *cp, char *arguments) 
{
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
