/**
 * @file read.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>

/**
 * @ingroup devcalls
 *
 * Read data from a device.
 *
 * @param descrp
 *      Index of the device to read from.
 * @param buffer
 *      Buffer into which to read the data.
 * @param count
 *      Maximum number of bytes to read.
 *
 * @return
 *      On success, returns the number of bytes read, which may be less than @p
 *      count in the event of a read error or end-of-file condition.
 *      Alternatively, ::SYSERR is returned if the device index is not valid or
 *      if a read error occurred before any data at all was successfully read.
 */
xinu_devcall read (int descrp, void *buffer, unsigned int count)
{
    device *devptr;
	if (!isbaddev(descrp))
	{
		devptr = (device *)&devtab[descrp];
		if (devptr && devptr->read) 
		{
			return ((*devptr->read) (devptr, buffer, count));
		}
	}
	return SYSERR;
}
