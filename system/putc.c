/**
 * @file putc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>

/**
 * @ingroup devcalls
 *
 * Write one character to a device.
 *
 * @param descrp
 *      Index of device to which to write the character.
 * @param ch
 *      character to write
 *
 * @return
 *      On success, returns the character written as an <code>unsigned
 *      char</code> cast to an @c int.  On bad device descripter, returns @c
 *      ::SYSERR.  On other failure, returns ::SYSERR or ::EOF depending on the
 *      specific device driver it calls.
 */
xinu_devcall putc (int descrp, char ch)
{
    device *devptr;
	if (!isbaddev(descrp))
	{
		devptr = (device *)&devtab[descrp];
		if (devptr && devptr->putc) 
		{
			return ((*devptr->putc) (devptr, ch));
		}
	}
	return SYSERR;
}
