/**
 * @file     ethloopWrite.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <bufpool.h>
#include <device.h>
#include <ethloop.h>
#include <CriticalSection.h>
#include <string.h>

/**
 * @ingroup ethloop
 *
 * Write data to an Ethernet Loopback device.  On success, the data will be
 * available to be read by a subsequent call to ethloopRead().
 *
 * @param devptr
 *      Pointer to the device table entry for the ethloop.
 *
 * @param buf
 *      Buffer of data to write.
 *
 * @param len
 *      Length of data to write, in bytes.
 *
 * @return
 *      On success, returns the number of bytes written, which will be exactly
 *      @p len.  On failure, returns SYSERR.
 */
xinu_devcall ethloopWrite (device *devptr, const void *buf, unsigned int len)
{
    struct ethloop *elpptr;
    int index;
    char *pkt;

    elpptr = &elooptab[devptr->minor];

    /* Make sure the packet isn't too small or too large  */
    if ((len < ELOOP_LINKHDRSIZE) || (len > ELOOP_BUFSIZE))
    {
        return SYSERR;
    }

	ENTER_KERNEL_CRITICAL_SECTION();

    /* Make sure the ethloop is actually open  */
    if (ELOOP_STATE_ALLOC != elpptr->state)
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    /* Drop packet if drop flags(s) are set */
    if (elpptr->flags & (ELOOP_FLAG_DROPNXT | ELOOP_FLAG_DROPALL))
    {
        elpptr->flags &= ~ELOOP_FLAG_DROPNXT;
		EXIT_KERNEL_CRITICAL_SECTION();
        return len;
    }

    /* Allocate buffer space.  This is blocking, so it can only fail if the pool
     * ID was corrupted.  */
    pkt = (char *)bufget(elpptr->poolid);
    if (SYSERR == (int)pkt)
    {
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    /* Copy supplied buffer into allocated buffer */
	memcpy(pkt, buf, len);

    /* Hold next packet if the appropriate flag is set */
    if (elpptr->flags & ELOOP_FLAG_HOLDNXT)
    {
        elpptr->flags &= ~ELOOP_FLAG_HOLDNXT;
        if (elpptr->hold != NULL)
        {
            buffree(elpptr->hold);
        }
        elpptr->hold = pkt;
        elpptr->holdlen = len;
		EXIT_KERNEL_CRITICAL_SECTION();
        signal(elpptr->hsem);
        return len;
    }

    /* Ensure there is enough buffer space (there always should be)  */
    if (elpptr->count >= ELOOP_NBUF)
    {
        buffree(pkt);
		EXIT_KERNEL_CRITICAL_SECTION();
        return SYSERR;
    }

    index = (elpptr->count + elpptr->index) % ELOOP_NBUF;

    /* Add to buffer */
    elpptr->buffer[index] = pkt;
    elpptr->pktlen[index] = len;
    elpptr->count++;

    /* Increment count of packets written */
    elpptr->nout++;

	EXIT_KERNEL_CRITICAL_SECTION();

    signal(elpptr->sem);

    return len;
}
