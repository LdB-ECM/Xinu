/* @file snoopCapture.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <snoop.h>

/**
 * @ingroup snoop
 *
 * Captures a network packet from a network interface.
 * @return OK if capture was successful, otherwise SYSERR
 */
int snoopCapture(struct snoop *cap, struct packet *pkt)
{
    struct packet *buf;
    int len;

    /* Error check pointers */
    if ((NULL == cap) || (NULL == pkt))
    {
        return SYSERR;
    }

    SNOOP_TRACE("Capturing packet");

    /* Increment count of packets captured */
    cap->ncap++;

    /* Check if packet matches capture filter, if not return OK */
    if (FALSE == snoopFilter(cap, pkt))
    {
        SNOOP_TRACE("Packet does not match filter");
        return OK;
    }

    /* Increment count of packets matching filter */
    cap->nmatch++;

    /* Try to get a buffer to put packet into */
    buf = netGetbuf();
    if (SYSERR == (int)buf)
    {
        SNOOP_TRACE("Failed to get buffer");
        return SYSERR;
    }

    /* Copy packet header into buffer */
	memcpy(buf, pkt, sizeof(struct packet));

    /* Copy packet contents into buffer */
    len = pkt->len;
    if (len > cap->caplen)
    {
        len = cap->caplen;
    }
	memcpy(buf->data, pkt->curr, len);
    buf->curr = buf->data;

    /* Queue packet */
    if (mailboxCount(cap->queue) >= SNOOP_QLEN)
    {
        netFreebuf(buf);
        cap->novrn++;
        SNOOP_TRACE("Capture queue full");
        return SYSERR;
    }
    if (SYSERR == mailboxSend(cap->queue, (int)buf))
    {
        netFreebuf(buf);
        SNOOP_TRACE("Failed to enqueue packet");
        return SYSERR;
    }

    return OK;
}
