/*
 * @file emuReorder.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <network.h>
#include <netemu.h>

/**
 * @ingroup netemu
 *
 * Reorder packets as specified by the user
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
xinu_syscall emuReorder(struct packet *pkt)
{

    return emuCorrupt(pkt);
}
