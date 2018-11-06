/**
 * @file     rawWrite.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <raw.h>

/**
 * @ingroup raw
 *
 * Write a packet to a raw socket.
 * @param devptr pointer to RAW device
 * @param buf buffer to write
 * @param len size of the buffer 
 * @return number of octets written, SYSERR if error occurred 
 */
xinu_devcall rawWrite(device *devptr, const void *buf, unsigned int len)
{
    struct raw *rawptr;

    rawptr = &rawtab[devptr->minor];
    return rawSend(rawptr, buf, len);
}
