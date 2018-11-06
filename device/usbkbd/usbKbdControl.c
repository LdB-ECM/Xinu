/**
 * @file usbKbdControl.c
 */
/* Embedded Xinu, Copyright (C) 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <usbkbd.h>
#include <CriticalSection.h>

/**
 * Control parameters to a USB keyboard.
 * @param devptr  pointer to USB keyboard device
 * @param func  index of function to run (defined in usbkbd.h)
 * @param arg1  first argument to called function
 * @param arg2  second argument to called function
 *
 * @return
 *      SYSERR if control function not recognized; otherwise a
 *      control-function-dependent value.
 */
xinu_devcall usbKbdControl(device *devptr, int func, long arg1, long arg2)
{
    struct usbkbd *kbd;
    int retval;

    USBKBD_TRACE("devptr->minor=%u, func=%d, arg1=%ld, arg2=%ld",
                 (uint)devptr->minor, func, arg1, arg2);

	ENTER_KERNEL_CRITICAL_SECTION();
    kbd = &usbkbds[devptr->minor];
    retval = SYSERR;

    if (!kbd->initialized)
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return retval;
    }

    switch (func)
    {
    case USBKBD_CTRL_SET_IFLAG:
        retval = kbd->iflags & arg1;
        kbd->iflags |= arg1;
        break;

    case USBKBD_CTRL_CLR_IFLAG:
        retval = kbd->iflags & arg1;
        kbd->iflags &= ~arg1;
        break;

    case USBKBD_CTRL_GET_IFLAG:
        retval = kbd->iflags;
        break;
    }
	EXIT_KERNEL_CRITICAL_SECTION();
    return retval;
}
