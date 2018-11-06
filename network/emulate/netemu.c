/*
 * @file netemu.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <network.h>
#include <netemu.h>

/**
 * @ingroup netemu
 *
 * Process a packet through the network emulator
 * @param pkt pointer to the incoming packet
 * @return OK if packet was processed succesfully, otherwise SYSERR
 */
xinu_syscall netemu(struct packet *pkt)
{

    return emuDrop(pkt);
}
