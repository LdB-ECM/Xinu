/**
 * @file getc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>

/**
 * @ingroup devcalls
 *
 * Read one character from a device.
 *
 * @param descrp
 *      Index of device from which to read the character.
 *
 * @return
 *      On success, returns the character read as an <code>unsigned char</code>
 *      cast to an @c int.  On bad device descripter, returns ::SYSERR.  On
 *      other failure, returns ::SYSERR or ::EOF depending on the specific
 *      device driver it calls.
 */
xinu_devcall getc (int descrp)
{
	if (!isbaddev(descrp))											// Check description id is valid
	{
		device *devptr = (device*)&devtab[descrp];					// Fetch device pointer
		if (devptr && devptr->getc)									// If pointer and function ptr valid
			return ((*devptr->getc) (devptr));						// Then call it and return result
	}
    return SYSERR;													// Any failure will return SYSERR
}
