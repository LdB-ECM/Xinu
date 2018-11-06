/**
 * @file     ttyWrite.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <tty.h>

/**
 * @ingroup tty
 *
 * Write a buffer to a tty
 * @param devptr TTY device table entry
 * @param buf buffer of characters to output
 * @param len size of the buffer
 * @return count of characters output
 */
xinu_devcall ttyWrite(device *devptr, const void *buf, unsigned int len)
{
    unsigned char ch = 0;
    unsigned int count = 0;
    unsigned char *buffer = (unsigned char*)buf;

    /* Write all characters in buffer */
    while (count < len)
    {
        ch = *buffer++;
        if (SYSERR == ttyPutc(devptr, ch))
        {
            return SYSERR;
        }
        count++;
    }

    return count;
}
