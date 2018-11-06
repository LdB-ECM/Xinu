/**
 * @file write.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <xinu.h>
#include <device.h>

/**
 * @ingroup devcalls
 *
 * Write data to a device.
 *
 * @param descrp
 *      Index of the device to write to.
 * @param buffer
 *      Buffer of data to write.
 * @param count
 *      Number of bytes of data to write.
 *
 * @return
 *      On success, returns the number of bytes written, which may be less than
 *      @p count in the event of a write error.  Alternatively, ::SYSERR is
 *      returned if the device index is not valid or if a write error occurred
 *      before any data at all was successfully written.
 *
 * Note that, depending on the specific device, this function may simply buffer
 * the data to be written at some later time via interrupt handling code.
 */
xinu_devcall write (int descrp, const void *buffer, unsigned int count)
{
    device *devptr;
	if (!isbaddev(descrp))
	{
		devptr = (device *)&devtab[descrp];
		if (devptr && devptr->write)
		{
			return ((*devptr->write) (devptr, buffer, count));
		}
	}
	return SYSERR;
}
