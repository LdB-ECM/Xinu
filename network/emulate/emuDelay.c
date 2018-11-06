/*
 * @file emuDelay.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <network.h>
#include <stdlib.h>
#include <thread.h>
#include <netemu.h>

/**
 * @ingroup netemu
 *
 * Delay packets as specified by user
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
xinu_syscall emuDelay(struct packet *pkt)
{
    /*
       short delayTime = 100;
       short delayPercent = 100;

       if ((rand() % 100) < delayPercent)
       {
       sleep(delayTime);
       }
     */
    return emuDuplicate(pkt);
}
