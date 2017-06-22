/* compile with "gcc -I $pcinc ..." to make IntHandler def available */
#include <stdio.h>
#include "tickpack.h"

IntHandler printstar;

/* We want a delay of over 10 secs, to see at least 2 stars */
/* Assuming about 4 instructions/loop, this will be about 12 secs */
#define CPU_MHZ 400000000
#define LOOPCOUNT CPU_MHZ

int main(void)
{
  int i,j;

  init_ticks();                        /* initialize the timer service */

  if (!set_timer(&printstar, 5, 5)) {  /* print star every 5 secs */
     printf("\nError: set Timer failed!\n");
     return 0;
  }

  if (set_timer(&printstar, 5, 5)) {   /* duplicate error */
     printf("\nError: set Timer duplicate detection failed!\n");
     return 0;
  }

  printf("Wait for a while to observe timer printouts.\n");
  for (i=0;i<LOOPCOUNT;i++)
    for(j=0;j<5;j++)
      ;

  if (!stop_timer(&printstar))  {     /* try to stop non-existant timer */
    printf("\nError: stop timer failed\n");
    return 0;
  }
  if (stop_timer(&printstar))   {     /* no timer set now */
    printf("\nError: stop timer for no timer failed\n");
    return 0;
  }

  printf("\nWait for a while to observe timer printouts have stopped.\n");
  for (i=0;i<LOOPCOUNT;i++)
    for(j=0;j<5;j++)
      ;

  shutdown_ticks();                  /* stop the timer service */
  printf("\nDone!\n");
  return 0;
}

void printstar()
{
  putchar('*');
}

