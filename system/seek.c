/**
 * @file seek.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>

/**
 * XXX:  This function apparently isn't used anywhere currently.
 *
 * position a device (very common special case of control)
 * @param descrp definition of device on which to seek
 * @param pos requested position to seek
 * @return function to seek on device on success, SYSERR on failure
 */
xinu_devcall seek (int descrp, unsigned int pos)
{
	xinu_devcall result = SYSERR;
    device *devptr;
	if (!isbaddev(descrp))
	{
		devptr = (device *)&devtab[descrp];
		if (devptr && devptr->seek)
		{
			return ((*devptr->seek) (devptr, pos));
		}
	}
	return result;
}
