/**
 * @file create.c
 */
/* Embedded Xinu, Copyright (C) 2007, 2013, 2018.  All rights reserved. */

#include <stdint.h>
#include <platform.h>
#include <string.h>
#include <thread.h>
#include <CriticalSection.h>

static int thrnew (void);

/**
 * @ingroup threads
 *
 * Create a thread to start running a procedure.
 *
 * @param procaddr
 *      procedure address
 * @param ssize
 *      stack size in bytes
 * @param priority
 *      thread priority (0 is lowest priority)
 * @param name
 *      name of the thread, used for debugging
 * @param nargs
 *      number of arguments that follow
 * @param ...
 *      arguments to pass to thread procedure
 * @return
 *      the new thread's thread id, or ::SYSERR if a new thread could not be
 *      created (not enough memory or thread entries).
 */
tid_typ create (void *procaddr, unsigned int ssize, int priority,
               const char *name, int nargs, ...)
{
    uintptr_t saddr;			/* stack address                       */
    tid_typ tid;                /* new thread ID                       */
    struct thrent *thrptr;      /* pointer to new thread control block */
    va_list ap;                 /* list of thread arguments            */



    if (ssize < MINSTK)												// Check req stack size not below minimum allowed size
    {
        ssize = MINSTK;														
    }
    saddr = (uintptr_t)stkget(ssize);								// Allocate a new stack
    if (SYSERR == (int) saddr)
    {
        return SYSERR;
    }

	ENTER_KERNEL_CRITICAL_SECTION();								// Need to disable pre-emptive scheduler as we play with shared things
    /* Allocate new thread ID.  */
    tid = thrnew();
    if (SYSERR == (int)tid)
    {
        stkfree(saddr, ssize);
		EXIT_KERNEL_CRITICAL_SECTION();								// We are going to exit so enable scheduler again
		return SYSERR;												// Return system error as no room in thread table
    }

    /* Set up thread table entry for new thread.  */
    thrcount++;
    thrptr = &thrtab[tid];

    thrptr->state = THRSUSP;
    thrptr->prio = priority;
    thrptr->stkbase = (void*)saddr;
    thrptr->stklen = ssize;
    strlcpy(thrptr->name, name, TNMLEN);
    thrptr->parent = gettid();
    thrptr->hasmsg = FALSE;
    thrptr->memlist.next = NULL;
    thrptr->memlist.length = 0;

    /* Set up default file descriptors.  */
    thrptr->fdesc[0] = CONSOLE; /* stdin  is console */
    thrptr->fdesc[1] = CONSOLE; /* stdout is console */
    thrptr->fdesc[2] = CONSOLE; /* stderr is console */
	thrptr->fdesc[3] = SERIAL0; /* kernel input is serial */
	thrptr->fdesc[4] = SERIAL0; /* kernel output is serial */

    /* Set up new thread's stack with context record and arguments.
     * Architecture-specific.  */
    va_start(ap, nargs);
    thrptr->stkptr = setupStack((void*)saddr, procaddr, INITRET, nargs, ap);
    va_end(ap);

	EXIT_KERNEL_CRITICAL_SECTION();								// We are exiting allow scheduler to operate
    return tid;													// Return the created thread id
}

/*
 * Obtain a new (free) thread ID.  Returns a free thread ID, or SYSERR if all
 * thread IDs are already in use.  This assumes IRQs have been disabled so that
 * the contents of the threads table are stable.
 */
static int thrnew(void)
{
    static int nexttid = 0;

    /* check all NTHREAD slots    */
    for (int tid = 0; tid < NTHREAD; tid++)
    {
        nexttid = (nexttid + 1) % NTHREAD;
        if (THRFREE == thrtab[nexttid].state)
        {
            return nexttid;
        }
    }
    return SYSERR;
}
