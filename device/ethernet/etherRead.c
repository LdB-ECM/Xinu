/**
 * @file etherRead.c
 */
/* Embedded Xinu, Copyright (C) 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <bufpool.h>
#include <ether.h>
#include <CriticalSection.h>
#include <string.h>

/* Implementation of etherRead() this function in ether.h.  */
xinu_devcall etherRead(device *devptr, void *buf, unsigned int len)
{
    struct ether *ethptr;
    struct ethPktBuffer *pkt;

	ENTER_KERNEL_CRITICAL_SECTION();

    /* Make sure device is actually up.  */
    ethptr = &ethertab[devptr->minor];
    if (ethptr->state != ETH_STATE_UP)
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    /* Wait for received packet to be available in the ethptr->in circular
     * queue.  */
    wait(ethptr->isema);

    /* Remove the received packet from the circular queue.  */
    pkt = ethptr->in[ethptr->istart];
    ethptr->istart = (ethptr->istart + 1) % ETH_IBLEN;
    ethptr->icount--;

    /* TODO: we'd like to restore interrupts here (before the memcpy()), but
     * this doesn't work yet because smsc9512_rx_complete() expects a buffer to
     * be available if icount < ETH_IBLEN; therefore, since we decremented
     * icount, we can't restore interrupts until we actually release the
     * corresponding buffer.  */

    /* Copy the data from the packet buffer, being careful to copy at most the
     * number of bytes requested. */
    if (pkt->length < len)
    {
        len = pkt->length;
    }
	memcpy(buf, pkt->buf, len);

    /* Return the packet buffer to the pool, then return the length of the
     * packet received.  */
    buffree(pkt);
	EXIT_KERNEL_CRITICAL_SECTION();
    return len;
}
