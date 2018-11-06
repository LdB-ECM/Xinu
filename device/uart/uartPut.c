/**
 * @file uartPutc.c
 */
 /* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <device.h>
#include <xinu.h>
#include <uart.h>

/**
 * @ingroup uartgeneric
 *
 * Write a single character to a UART.
 *
 * @param devptr
 *      Pointer to the device table entry for a UART.
 * @param ch
 *      The character to write.
 *
 * @return
 *      On success, returns the character written as an <code>unsigned
 *      char</code> cast to an @c int.  On failure, returns SYSERR.
 */
xinu_devcall uartPutc(device *devptr, char ch)
{
	int retval;

	retval = uartWrite(devptr, &ch, 1);
	if (retval == 1)
	{
		return (unsigned char)ch;
	}
	else
	{
		return SYSERR;
	}
}
