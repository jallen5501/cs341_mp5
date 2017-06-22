/* comintspack.h */
#ifndef COMINTSPACK_H
#define COMINTSPACK_H

/* mode values */
#define TRANSMIT 0
#define RECEIVE 1

/* get the UART to start TX or RX interrupts based on mode */
void init_comints (int mode, void (*callback)(char *), char *buffer, int size);

/* shut down the com port interrupts */
void shutdown_comints (void);

#endif

