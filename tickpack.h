#ifndef TICKPACK_H
#define TICKPACK_H
#include <cpu.h>

/* Start ticking service */
void init_ticks(void);

/* Activate a timer */
int set_timer(IntHandler *function, int time, int reinittime);

/* Deactivate a timer */
int stop_timer(IntHandler *function);

/* Shut down ticking service */
void shutdown_ticks(void);

#endif
