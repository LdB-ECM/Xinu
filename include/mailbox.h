/**
 * @file mailbox.h
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#ifndef _MAILBOX_H_
#define _MAILBOX_H_

#include <xinu.h>
#include <semaphore.h>
#include <conf.h>

#define MAILBOX_FREE     0
#define MAILBOX_ALLOC    1

/**
 * Defines what an entry in the mailbox table looks like.
 */
struct mbox
{
    semaphore sender;           /**< count of free spaces in mailbox    */
    semaphore receiver;         /**< count of messages ready to recieve */
    unsigned int max;           /**< max #of messages mailbox can hold  */
    unsigned int count;         /**< #of msgs currently in mailbox      */
    unsigned int start;         /**< index into buffer of first msg     */
    unsigned char state;        /**< state of the mailbox               */
    int *msgs;                  /**< message queue for the mailbox      */
};

typedef unsigned int mailbox;

extern semaphore mboxtabsem;

extern struct mbox mboxtab[];

/* Mailbox function prototypes */
xinu_syscall mailboxAlloc(unsigned int);
xinu_syscall mailboxCount(mailbox);
xinu_syscall mailboxFree(mailbox);
xinu_syscall mailboxInit(void);
xinu_syscall mailboxReceive(mailbox);
xinu_syscall mailboxSend(mailbox, int);

#endif                          /* _MAILBOX_H_ */
