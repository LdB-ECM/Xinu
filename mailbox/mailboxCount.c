/**
 * @file mailboxCount.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <CriticalSection.h>
#include <mailbox.h>

/**
 * @ingroup mailbox
 *
 * Retrieve the number of outstanding messages in the specified mailbox.
 *
 * @param box
 *      The index of the mailbox for which to retrieve the number of outstanding
 *      messages.
 *
 * @return
 *      The number of messages in the mailbox, or ::SYSERR if @p box did not
 *      specify a valid allocated mailbox.
 */
xinu_syscall mailboxCount(mailbox box)
{
    const struct mbox *mbxptr;
    int retval;

    if (!(0 <= box && box < NMAILBOX))
    {
        return SYSERR;
    }

    mbxptr = &mboxtab[box];
	ENTER_KERNEL_CRITICAL_SECTION();
    if (MAILBOX_ALLOC == mbxptr->state)
    {
        retval = mbxptr->count;
    }
    else
    {
        retval = SYSERR;
    }
	EXIT_KERNEL_CRITICAL_SECTION();
    return retval;
}
