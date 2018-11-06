/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <CriticalSection.h>
#include <semaphore.h>
#include <stdlib.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Delete a TCB.
 * @param tcbptr pointer to transmission control block for connection
 * @precondition TCB mutex is already held
 */
xinu_devcall tcpFree(struct tcb *tcbptr)
{
    semaphore temp;

    /* Verify TCB is not already free */
    if (TCP_CLOSED == tcbptr->state)
    {
        signal(tcbptr->mutex);
        return SYSERR;
    }

    TCP_TRACE("Free TCB");

	ENTER_KERNEL_CRITICAL_SECTION();

    /* Free TCB */
    temp = tcbptr->mutex;
    semfree(tcbptr->openclose);
    semfree(tcbptr->readers);
    semfree(tcbptr->writers);
    tcpTimerPurge(tcbptr, NULL);
    bzero(tcbptr, sizeof(struct tcb));  /* Clear tcp structure. */
    tcbptr->state = TCP_CLOSED;
    tcbptr->devstate = TCP_FREE;
    tcbptr->mutex = temp;
	EXIT_KERNEL_CRITICAL_SECTION();
    signal(tcbptr->mutex);
    return OK;
}
