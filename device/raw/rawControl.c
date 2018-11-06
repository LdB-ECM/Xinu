/**
 * @file rawControl.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <raw.h>
#include <CriticalSection.h>

/**
 * @ingroup raw
 *
 * Control function for raw sockets.
 * @param devptr RAW device table entry
 * @param func control function to execute
 * @param arg1 first argument for the control function
 * @param arg2 second argument for the control function
 * @return the result of the control function
 */
xinu_devcall rawControl(device *devptr, int func, long arg1, long arg2)
{
    struct raw *rawptr;
    unsigned char old;

    rawptr = &rawtab[devptr->minor];
	ENTER_KERNEL_CRITICAL_SECTION();
    /* Check if raw socket is open */
    if (RAW_ALLOC != rawptr->state)
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    switch (func)
    {
        /* Set flags: arg1 = flags to set      */
        /* return old value of flags                 */
    case RAW_CTRL_SETFLAG:
        old = rawptr->flags & arg1;
        rawptr->flags |= (arg1);
		EXIT_KERNEL_CRITICAL_SECTION();
        return old;

        /* Clear flags: arg1 = flags to clear  */
        /* return old value of flags                 */
    case RAW_CTRL_CLRFLAG:
        old = rawptr->flags & arg1;
        rawptr->flags &= ~(arg1);
		EXIT_KERNEL_CRITICAL_SECTION();
        return old;
    }

	EXIT_KERNEL_CRITICAL_SECTION();
    return SYSERR;
}
