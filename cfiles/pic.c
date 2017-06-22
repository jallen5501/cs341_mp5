/* pic.c: run PIC (programmable interrupt controller) */
/* Note that we are depending on Tutor to initialize the PIC properly
   at bootup, so all we need are actions to change individual irqs */
#include <pic.h>

/* Command PIC to let signals on line for specified irq through to CPU
   Works for irqs 0-15, except 2, which is reserved for cascading to
   the slave chip.  For irqs 8-15, we need to use the SLAVE chip */
void pic_enable_irq(int irq)
{
  unsigned int irqmask;

  /* tell PIC to interrupt CPU for this irq line */
  if (irq==2)
    return;
  if (irq < 8) {
    irqmask = inpt(PIC_MASTER1); /* read irq masks for irq 0-7 */
    outpt(PIC_MASTER1, irqmask & ~(1<<irq)); /* clear bit irq to enable line */
  } else {
    irqmask = inpt(PIC_SLAVE1);	/* read irq masks for irq 8-15 */
    outpt(PIC_SLAVE1, irqmask & ~(1<<(irq-8))); /* clear bit irq to enable line */
  }
}

/* Command PIC to stop signals on line irq from reaching CPU.
   Works for irqs 0-15 except 2 */
void pic_disable_irq(int irq)
{
  unsigned int irqmask;
  
  if (irq==2)
    return;
  if (irq<8) {
    irqmask = inpt(PIC_MASTER1); /* read irq masks for irq 0-7 */
    outpt(PIC_MASTER1, irqmask|(1<<irq)); /* set bit irq to disable line*/
  } else {
    irqmask = inpt(PIC_SLAVE1);	/* read irq masks for irq 8-15 */
    outpt(PIC_SLAVE1, irqmask|(1<<(irq-8))); /* set bit irq to disable line*/
  }
}

/* Tell PIC that this interrupt is being handled, so it can work on another.
   This code works for irqs 0-7.  Above that, you need to tell *both*
   MASTER and SLAVE chips */
void pic_end_int()
{
  outpt(PIC_MASTER0,PIC_EOI);	/* EOI: end of interrupt notification to PIC */
}

/* Tell PIC that this interrupt is being handled, so it can work on another.
   This code works for irqs 8-15, also lower ones. */
void pic_end_int2()
{
  outpt(PIC_MASTER0,PIC_EOI);	/* EOI: end of interrupt notification to PIC */
  outpt(PIC_SLAVE0,PIC_EOI);	/* EOI: end of interrupt notification to PIC */
}
