/**
 * @file ttyClose.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <tty.h>
#include <stdlib.h>
#include <CriticalSection.h>

/**
 * @ingroup tty
 *
 * Close a TTY device.
 * @param devptr TTY device table entry
 * @return OK if TTY is closed properly, otherwise SYSERR
 */
xinu_devcall ttyClose(device *devptr)
{
    struct tty *ttyptr;

	ENTER_KERNEL_CRITICAL_SECTION();
    /* Setup and error check pointers to structures */
    ttyptr = &ttytab[devptr->minor];
    bzero(ttyptr, sizeof(struct tty));  /* Clear TTY structure.         */
	EXIT_KERNEL_CRITICAL_SECTION();
    return OK;
}
