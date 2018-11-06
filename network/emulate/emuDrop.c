/*
 * @file emuDrop.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <stdlib.h>
#include <network.h>
#include <netemu.h>
#include <route.h>
#include <date.h>
#include <stdio.h>
#include <udp.h>

/**
 * @ingroup netemu
 *
 * Drop packets based on user settings
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
xinu_syscall emuDrop(struct packet *pkt)
{
    /* TODO: make this user specifiable */
    short dropPercent = 0;

    /* drop packet when random value < percent to drop */
    if ((rand() % 100) < dropPercent)
    {
        RT_TRACE("Dropped by Emulator: %d\n");
        netFreebuf(pkt);
        return OK;
    }

    return emuDelay(pkt);
}
