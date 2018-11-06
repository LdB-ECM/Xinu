/**
 * @file tcpOpen.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <network.h>
#include <semaphore.h>
#include <stdarg.h>
#include <tcp.h>

static unsigned short allocPort(void);

/**
 * @ingroup tcp
 *
 * Associate a tcp with a network device.
 * @param devptr tcp device table entry
 * @param ap 2nd argument is the local IP address
 *           3rd argument is the remote IP address
 *           4th argument is the local port (auto-assigned if zero)
 *           5th argument is the remote port (ignored if zero)
 *           6th argument is the mode (TCP_ACTIVE or TCP_PASSIVE)
 * @return OK if TCP is opened properly, otherwise SYSERR
 */
xinu_devcall tcpOpen(device *devptr, va_list ap)
{
    struct tcb *tcbptr = NULL;
    unsigned char mode;
    unsigned short localpt;
    struct netaddr *localip;
    unsigned short remotept;
    struct netaddr *remoteip;

    /* Setup pointer to tcp */
    tcbptr = &tcptab[devptr->minor];

    wait(tcbptr->mutex);

    /* Mark as allocated */
    tcbptr->devstate = TCP_ALLOC;

    /* Verify device is not already open */
    if ((tcbptr->state != TCP_CLOSED) && (tcbptr->state != TCP_LISTEN))
    {
        signal(tcbptr->mutex);
        TCP_TRACE("Already open");
        return SYSERR;
    }

    /* Obtain remaining arguments */
    localip = (struct netaddr *)va_arg(ap, unsigned int);
    remoteip = (struct netaddr *)va_arg(ap, unsigned int);
    localpt = va_arg(ap, unsigned int);
    remotept = va_arg(ap, unsigned int);
    mode = va_arg(ap, unsigned int);

    /* Verify local IP is specified */
    if (NULL == localip)
    {
        tcbptr->devstate = TCP_FREE;
        signal(tcbptr->mutex);
        TCP_TRACE("Invalid args");
        return SYSERR;
    }

    /* Allocate a local port if none is specified */
    if (NULL == localpt)
    {
        localpt = allocPort();
    }

    /* Mutually link tcp record with device table entry */
    tcbptr->dev = devptr->num;

    /* Initialize port and ip fields */
    tcbptr->localpt = localpt;
    netaddrcpy(&tcbptr->localip, localip);
    tcbptr->remotept = remotept;
    if (NULL == remoteip)
    {
        tcbptr->remoteip.type = NULL;
    }
    else
    {
        netaddrcpy(&tcbptr->remoteip, remoteip);
    }
    tcbptr->opentype = mode;

    /* Setup transmission control block */
    if (SYSERR == tcpSetup(tcbptr))
    {
        tcpFree(tcbptr);
        TCP_TRACE("Failed to setup TCB");
        return SYSERR;
    }

    /* Perform appropriate action and change state */
    switch (mode)
    {
    case TCP_PASSIVE:
        tcbptr->state = TCP_LISTEN;
        break;
    case TCP_ACTIVE:
        if (SYSERR == tcpOpenActive(tcbptr))
        {
            tcpFree(tcbptr);
            TCP_TRACE("Failed to active open");
            return SYSERR;
        }
        break;
    default:
        tcpFree(tcbptr);
        TCP_TRACE("Unknown mode");
        return SYSERR;
    }

    signal(tcbptr->mutex);

    TCP_TRACE("Waiting for other side");
    wait(tcbptr->openclose);    /* Wait for connection open */
    TCP_TRACE("Opened connection");
    return OK;
}

/**
 * Allocates an unused TCP port to use as a local port
 * @return TCP port
 */
static unsigned short allocPort()
{
    static unsigned short nextport = 0;
    int i = 0;

    nextport = (nextport + 1) % (TCP_PMAX - TCP_PSTART);
    for (i = 0; i < NTCP; i++)
    {
        if ((tcptab[i].state != TCP_CLOSED)
            && (tcptab[i].localpt == (nextport + TCP_PSTART)))
        {
            nextport = (nextport + 1) % (TCP_PMAX - TCP_PSTART);
            i = 0;
        }
    }

    return (nextport + TCP_PSTART);
}
