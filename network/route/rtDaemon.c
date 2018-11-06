/**
 * @file rtDaemon.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <mailbox.h>
#include <network.h>
#include <route.h>
#include <thread.h>

/**
 * @ingroup route
 *
 * Route dameon to route packets
 */
thread rtDaemon(void)
{
    struct packet *pkt = NULL;

    while (TRUE)
    {
        pkt = (struct packet *)mailboxReceive(rtqueue);
        RT_TRACE("Daemon received packet");
        if (SYSERR == (int)pkt)
        {
            RT_TRACE("Daemon received packet has an error");
            continue;
        }

        rtSend(pkt);
        if (SYSERR == netFreebuf(pkt))
        {
            RT_TRACE("Failed to free packet buffer");
            continue;
        }
    }

    return OK;
}
