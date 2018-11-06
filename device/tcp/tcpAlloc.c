/**
 * @file tcpAlloc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <semaphore.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Allocate an available tcp device.
 * @return device number for a tcp device, SYSERR if none are free
 */
unsigned short tcpAlloc(void)
{
    int i;

    for (i = 0; i < NTCP; i++)
    {
        wait(tcptab[i].mutex);
        if (TCP_FREE == tcptab[i].devstate)
        {
            tcptab[i].devstate = TCP_ALLOC;
            signal(tcptab[i].mutex);
            return i + TCP0;
        }
        signal(tcptab[i].mutex);
    }
    return SYSERR;
}
