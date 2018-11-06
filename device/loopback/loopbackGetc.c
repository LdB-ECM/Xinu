/**
 * @file     loopbackGetc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <semaphore.h>
#include <loopback.h>
#include <stdio.h>
#include <CriticalSection.h>

/**
 * @ingroup loopback
 *
 * Get a character from the loopback buffer, possibly blocking.
 *
 * @param devptr
 *      Pointer to the loopback device.
 *
 * @return
 *      The resulting @p ch as an <code>unsigned char</code> cast to an @c int
 *      on success; @c EOF if there is no data available and the device is in
 *      nonblocking mode.
 */
xinu_devcall loopbackGetc (device *devptr)
{
    struct loopback *lbkptr;
    unsigned char ch;

    lbkptr = &looptab[devptr->minor];

	ENTER_KERNEL_CRITICAL_SECTION();

    /* wait until the buffer has data */
    if (LOOP_NONBLOCK == (lbkptr->flags & LOOP_NONBLOCK)) {
        if (semcount(lbkptr->sem) <= 0) {
			EXIT_KERNEL_CRITICAL_SECTION();
            return EOF;
        }
    }

    wait(lbkptr->sem);

    /* Get and return the next character.  */
    ch = lbkptr->buffer[lbkptr->index];
    lbkptr->index = (lbkptr->index + 1) % LOOP_BUFFER;
	EXIT_KERNEL_CRITICAL_SECTION();
    return ch;
}
