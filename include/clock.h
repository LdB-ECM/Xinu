/**
 * @file clock.h
 * Definitions relating to the hardware clock and the Xinu clock interrupt.
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <xinu.h>
#include <queue.h>

/**
 * @ingroup timer
 *
 * Timer resolution.  This refers to the maximum number of times that Xinu
 * schedules the timer interrupt per second, @a not to the number of native
 * clock cycles per second.
 */
#define CLKTICKS_PER_SEC  1000

extern volatile unsigned long clkticks;
extern volatile unsigned long clktime;
extern qid_typ sleepq;

/* Clock function prototypes.  Note:  clkupdate() and clkcount() are documented
 * here because their implementations are platform-dependent.  */

void clkinit (void);

/**
 * @ingroup timer
 *
 * Sets up a timer interrupt to trigger after a certain number of clock cycles
 * have elapsed.
 *
 * @param cycles
 *     Number of cycles after which the timer interrupt is to be triggered.
 *     This refers to native clock cycles (whose frequency is specified in
 *     platform::clkfreq).
 */
void clkupdate (unsigned long cycles);

/**
 * @ingroup timer
 *
 * Gets the current system timer cycles.
 *
 * @return
 *	The current number of timer cycles.  Generally, only the difference between
 *	the value returned by between two successive calls to this function is
 *	meaningful.  The number of cycles that elapse per second is specified by the
 *	@ref platform::clkfreq "value in platform".
 */
unsigned long clkcount( void);

interrupt clkhandler(void);
void udelay (unsigned long);
void mdelay (unsigned long);

#endif                          /* _CLOCK_H_ */
