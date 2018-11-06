/**
 * @file interrupt.h
 * 
 * Constants and declarations associated with interrupt handling.
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include <xinu.h>


/* System timer - one IRQ line per output compare register.  */
#define IRQ_SYSTEM_TIMER_0 0
#define IRQ_SYSTEM_TIMER_1 1
#define IRQ_SYSTEM_TIMER_2 2
#define IRQ_SYSTEM_TIMER_3 3

/* Timer IRQ to use by default.  Note: this only be either 1 or 3, since 0 and 2
 * are already used by the VideoCore.  */
#define IRQ_TIMER          IRQ_SYSTEM_TIMER_3

 /* Synopsys DesignWare Hi-Speed USB 2.0 On-The-Go Controller  */
#define IRQ_USB            9

/* PL011 UART  */
#define IRQ_MINIUART       29

/* PCM sound  */
#define IRQ_PCM            55

/* PL011 UART  */
#define IRQ_PL011          57

/* SD card host controller  */
#define IRQ_SD             62


typedef interrupt (*interrupt_handler_t)(void);

typedef unsigned long irqmask;  /**< machine status for disable/restore  */


irqmask disable(void);
irqmask restore(irqmask);
void enable_irq(irqmask);
void disable_irq(irqmask);

int set_interrupt_handler(unsigned int intnum, interrupt_handler_t handler);

#endif /* _INTERRUPT_H_ */
