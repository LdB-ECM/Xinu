/**
 * @file     clkinit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#if RTCLOCK

#include <kernel.h>
#include <xinu.h>
#include <platform.h>
#include <interrupt.h>
#include <clock.h>
#include <queue.h>

/** @ingroup timer
 *
 * Number of timer interrupts that have occurred since ::clktime was
 * incremented.  When ::clkticks reaches ::CLKTICKS_PER_SEC, ::clktime is
 * incremented again and ::clkticks is reset to 0.
 */
volatile unsigned long clkticks;

/** @ingroup timer
 * Number of seconds that have elapsed since the system booted.  */
volatile unsigned long clktime;

/** Queue of sleeping processes.  */
qid_typ sleepq;

extern void clockIRQ(void);
#define CLOCKBASE 0x40         /* I/O base port of clock chip for x86 */
#define CLOCKCTL (CLOCKBASE+3) /* chip CSW I/O port for x86           */
ulong time_intr_freq = 0;     /** frequency of XINU clock interrupt   */

/**
 * @ingroup timer
 *
 * Initialize the clock and sleep queue.  This function is called at startup.
 */
void clkinit(void)
{
    sleepq = queinit();         /* initialize sleep queue       */

    clkticks = 0;

#ifdef DETAIL
    kprintf("Time base %dHz, Clock ticks at %dHz\r\n",
            platform.clkfreq, CLKTICKS_PER_SEC);
#endif

	time_intr_freq = platform.clkfreq / CLKTICKS_PER_SEC;
	outb(CLOCKCTL, 0x34);
	/* LSB then MSB */
	outb(CLOCKBASE, time_intr_freq);
	outb(CLOCKBASE, time_intr_freq >> 8);
	outb(CLOCKBASE, time_intr_freq >> 8); /* why??? */
	set_evec(IRQBASE, (unsigned long)clockIRQ);

}

#endif                          /* RTCLOCK */
