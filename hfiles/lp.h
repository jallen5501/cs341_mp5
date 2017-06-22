/* lp.h: definitions for accessing I/O ports for LPTn */
#ifndef LP_H
#define LP_H
/*
 * partly from:
 * usr/include/linux/lp.h c.1991-1992 James Wiegand
 * many modifications copyright (C) 1992 Michael K. Johnson
 * Interrupt support added 1993 Nigel Gamble
 * Edited for UMB 1996 Betty O'Neil
 */

#define LPT1_BASE 0x378
/* for PCs with second parallel port */
#define LPT2_BASE 0x278
#define LPT1_IRQ 7
/* LP ports, as offsets from the base register */
#define LP_DATA        0       /* Out, In: 8 bits of data */
#define LP_STATUS      1       /* In: status bits, see below */
#define LP_CNTRL       2       /* In/Out: control bits, see below */

/* hardware part of linux lp.h, slightly edited.  Also see S&S, p. 628
 */
/*
 * The following constants describe the various signals of the printer port
 * hardware.  Note that the hardware inverts some signals and that some
 * signals are active low.  An example is LP_STROBE, which must be programmed
 * with 1 for being active and 0 for being inactive, because the strobe signal
 * gets inverted, but it is also active low (pin=0 means active).
 */

/* 
 * bit defines for the status port: except for PIRQSTAT and inversion of
 *    PBUSY, these bits read from port 379 just reflect the current signals 
 *    on the external port pins
 * base + 1
 */
#define LP_PBUSY	0x80  /* BUSY pin: inverted input, active high */
#define LP_PACK		0x40  /* ACK pin: unchanged input, active low */
#define LP_POUTPA	0x20  /* PE pin: unchanged input, active high */
#define LP_PSELECD	0x10  /* SLCT pin: unchanged input, active high */
#define LP_PERRORP	0x08  /* ERROR pin: unchanged input, active low */
#define LP_PIRQSTAT     0x04  /* IRQ status: if 0, interrupt pending */
/* 
 * bit defines for the control port: PDIR and PINTEN are interpreted
 *  by the device, all others just pass through to external port,
 *  in some cases with inversion along the way.
 * base + 2 
 */
#define LP_PDIR         0x20  /* set to read in data from bidir port */
#define LP_PINTEN	0x10  /* interrupt enable */
#define LP_PSELECP	0x08  /* SLCTIN pin: inverted output, active low */
#define LP_PINITP	0x04  /* INIT pin: unchanged output, active low */
#define LP_PAUTOLF	0x02  /* AUTOFD pin: inverted output, active low */
#define LP_PSTROBE	0x01  /* STROBE pin: inverted output, active low */

#endif
