/**
 * @file close.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>

/**
 * @ingroup devcalls
 *
 * Close a device.
 *
 * @param descrp
 *      Index of the device to close.
 *
 * @return
 *      ::OK if device was successfully closed; ::SYSERR otherwise.
 *
 * Most device drivers will return ::SYSERR if the device is not open but
 * otherwise will always be able to successfully close the device and return
 * ::OK.
 *
 * Caveat:  You must not close the device while any threads are using it (e.g.
 * for read() or write()), unless the corresponding device driver documents this
 * as allowed.
 */
xinu_devcall close (int descrp)
{
    if (!isbaddev(descrp))											// Check description id is valid
	{
		device *devptr = (device *)&devtab[descrp];					// Fetch device pointer
		if (devptr && devptr->close)								// If pointer and function ptr valid
			return ((*devptr->close) (devptr));						// Then call it and return result
	}
    return SYSERR;													// Any failure will return SYSERR
}
