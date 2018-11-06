/**
 * @file tcpGetc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Read into a single octet from TCP.
 * @param devptr TCP device table entry
 * @return character read from TCP
 */
xinu_devcall tcpGetc(device *devptr)
{
    unsigned char ch;
    int result = 0;

    result = tcpRead(devptr, &ch, 1);

    if (result != 1)
    {
        return result;
    }

    return ch;
}
