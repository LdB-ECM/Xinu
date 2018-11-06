/**
 * @file mailboxSend.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <CriticalSection.h>
#include <mailbox.h>

/**
 * @ingroup mailbox
 *
 * Send a message to the specified mailbox.
 *
 * @param box
 *      The index of the mailbox to send the message to.
 *
 * @param mailmsg
 *      The message to send.
 *
 * @return ::OK if the message was successfully enqueued, otherwise ::SYSERR.
 *         ::SYSERR is returned only if @p box did not specify a valid allocated
 *         mailbox or if the mailbox was freed while waiting for room in the
 *         queue.
 */
xinu_syscall mailboxSend(mailbox box, int mailmsg)
{
    struct mbox *mbxptr;
    int retval;

    if (!(0 <= box && box < NMAILBOX))
    {
        return SYSERR;
    }

    mbxptr = &mboxtab[box];
	ENTER_KERNEL_CRITICAL_SECTION();
    retval = SYSERR;
    if (MAILBOX_ALLOC == mbxptr->state)
    {
        /* wait until there is room in the mailmsg queue */
        wait(mbxptr->sender);

        /* only continue if the mailbox hasn't been freed  */
        if (MAILBOX_ALLOC == mbxptr->state)
        {
            /* write mailmsg to this mailbox's mailmsg queue */
            mbxptr->msgs[((mbxptr->start + mbxptr->count) % mbxptr->max)] =
                mailmsg;
            mbxptr->count++;

            /* signal that there is another mailmsg in the mailmsg queue */
            signal(mbxptr->receiver);

            retval = OK;
        }
    }

	EXIT_KERNEL_CRITICAL_SECTION();
    return retval;
}
