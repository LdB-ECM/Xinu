/**
 * @file control.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>

/**
 * @ingroup devcalls
 *
 * Execute an I/O Control request on a device.
 *
 * @param descrp
 *      Index of the device.
 * @param func
 *      Device-specific specific control "function".
 * @param arg1
 *      Additional argument for the device-specific control function.
 * @param arg2
 *      Additional argument for the device-specific control function.
 *
 * @return
 *      Returns ::SYSERR if the device index does not correspond to an
 *      appropriate device or if the control function is not recognized;
 *      otherwise returns a request-specific value that is typically ::SYSERR on
 *      failure, but may be either ::OK or request-specific data on success.
 */
xinu_devcall control (int descrp, int func, long arg1, long arg2)
{
    if (!isbaddev(descrp))											// Check description id is valid
    {
		device *devptr = (device *)&devtab[descrp];					// Fetch device pointer
		if (devptr && devptr->control)								// If pointer and function ptr valid
			return ((*devptr->control) (devptr, func, arg1, arg2)); // Call function and return result
    }
	return SYSERR;													// Any failure will return SYSERR
}
