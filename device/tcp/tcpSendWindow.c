/**
 * @file tcpSendWindow.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Calculates the window size to advertise in an outgoing TCP packet.
 * @param tcbptr pointer to transmission control block for connection
 * @pre-condition TCB mutex is already held
 * @post-condition TCB mutex is still held
 */
unsigned short tcpSendWindow(struct tcb *tcbptr)
{
    unsigned short unused = 0;
    unsigned short window = 0;

    /* Set proposed window to maximum possible */
    window = TCP_IBLEN - tcbptr->icount;        // Correct?

    switch (tcbptr->state)
    {
    case TCP_CLOSED:
        /* Should not be in CLOSED state */
        return 0;
    case TCP_LISTEN:
    case TCP_SYNSENT:
    case TCP_SYNRECV:
        /* Don't do receiver-side silly window syndrome avoidance */
        tcbptr->rcvwnd = seqadd(tcbptr->rcvnxt, window);
        return window;
    }

    /* Receiver-side silly window syndrome avoidance */
    /* Calculate unsued portion of currently advertised window */
    unused = (unsigned short)tcpSeqdiff(tcbptr->rcvwnd, tcbptr->rcvnxt);
#ifdef TCP_FAKEACK
    if (seqlt(tcbptr->rcvwnd, tcbptr->rcvnxt))
    {
        unused = 0;
    }
#endif
    /* Use 0 if proposed window less than 1/4 buffer or less than 1 MSS */
    if (((window * 4) < TCP_IBLEN) || (window < tcbptr->rcvmss))
    {
        window = 0;
    }
    /* If proposed win is greater than unused advertised win, use new size */
    if (window > unused)
    {
        tcbptr->rcvwnd = seqadd(tcbptr->rcvnxt, window);
        return window;
    }
    /* Otherwise, keep current advertised window end point */
    return unused;
}
