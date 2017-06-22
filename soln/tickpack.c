/* tickpack.c 
 * Bob Wilson
 * 05/11/2003
 * A timer package for PC-Tutor
*/
#include <stdio.h>
#include "tickpack.h"
#include <timer.h>
#include <pic.h>
#include <cpu.h>

extern IntHandler irq0inthand;

#define ENTRIES 5

/* internal function prototypes */
void set_timer_count(int);
void smalldelay(void);
void irq0inthandc(void);

typedef struct entry {
	IntHandler *callback;
	int time;
	int reinit_time;
}time_table_entry;

static time_table_entry time_table[ENTRIES];

void init_ticks() {
  /* clear the timer table */
  time_table_entry *current = time_table;
  int i;  

  for (i = 0; i < ENTRIES; i++) {
    current->callback = NULL;
    current->time = 0;
    current->reinit_time = 0;
    current++;   
  } 

#ifdef DEBUG
  printf("Disabling interrupts in CPU while setting them up\n");
#endif
  cli();
#ifdef DEBUG  
  printf("Setting interrupt gate for timer, irq 0\n");
#endif
  /* irq 0 maps to slot n = 0x20 in IDT for linux setup */
  set_intr_gate(TIMER0_IRQ+IRQ_TO_INT_N_SHIFT, &irq0inthand);
#ifdef DEBUG
  printf("Commanding PIC to interrupt CPU for irq 0\n");
#endif
  pic_enable_irq(TIMER0_IRQ);
#ifdef DEBUG
  printf("Commanding timer to generate a pulse train using max count\n");
#endif
  set_timer_count(0);   
#ifdef DEBUG
  printf("Enabling interrupts in CPU\n");
#endif
  sti();
}

int set_timer(IntHandler *callback, int time, int reinit_time)
{
  time_table_entry *current = time_table;
  int i;

  for (i = 0; i < ENTRIES; i++)
    if (current->callback == callback)
      return 0;     /* Error: duplicate timer entry */

  current = time_table;
  for (i = 0; i < ENTRIES; i++) {
    if (current->callback == NULL) {
      current->callback = callback;
      current->time = 18 * time;
      current->reinit_time = 18 * reinit_time;
      return 1;    /* OK */
    }
    current++;
  }
  return 0;        /* Error: table full */
}

int stop_timer(IntHandler *function)
{
  time_table_entry *current = time_table;
  int i;
  
  for (i = 0; i < ENTRIES; i++) {
    if (current->callback == function) {
      current->callback = NULL;     
      current->time = 0;
      current->reinit_time = 0;
      return 1;    /* OK */
    }
    current++;
  }
  return 0;        /* Error: timer was not set */
  }

void shutdown_ticks()
{
  cli();
#ifdef DEBUG
  printf("Commanding PIC to shut off irq 0 to CPU\n");
#endif
  pic_disable_irq(TIMER0_IRQ);  /* disallow irq 0 ints to get to CPU */
  sti();
}

/* about 10 us on a SAPC (400Mhz Pentium) */
void smalldelay(void)
{
  int i;
    
  for (i=0;i<1000;i++)
     ;
}

/* Set up timer to count down from given count, then send a tick interrupt, */
/* over and over. A count of 0 sets max count, 65536 = 2**16 */
void set_timer_count(int count)
{
  outpt(TIMER_CNTRL_PORT, TIMER0|TIMER_SET_ALL|TIMER_MODE_RATEGEN);
  outpt(TIMER0_COUNT_PORT,count&0xff); /* set LSB here */
  outpt(TIMER0_COUNT_PORT,count>>8); /* and MSB here */
  smalldelay();                 /* give the timer a moment to init. */
}

/* timer interrupt handler */
void irq0inthandc(void)
{
  time_table_entry *current = time_table;
  int i;
 
  pic_end_int();                /* notify PIC that its part is done */
  for (i = 0; i <ENTRIES; i++) {
    if (current->callback != NULL && current->time > 0) {
      current->time--;
      if (current->time == 0) {
        (*current->callback) ();
        current->time = current->reinit_time;
      }
    }
    current++;
  }
}

