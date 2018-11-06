/**
 * @file monitor.h
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <xinu.h>
#include <thread.h>
#include <semaphore.h>

#ifndef NMON
#  define NMON 0
#endif

/* Monitor state definitions */
#define MFREE 0x01 /**< this monitor is free */
#define MUSED 0x02 /**< this monitor is used */

#define NOOWNER BADTID /**< no thread owns this monitor's lock */

/** type definition of "monitor" */
typedef unsigned int monitor;

/**
 * Monitor table entry
 */
struct monent
{
    char state;       /**< monitor state (MFREE or MUSED)  */
    tid_typ owner;    /**< thread that owns the lock, or NOOWNER if unowned  */
    unsigned int count;       /**< number of lock actions performed  */
    semaphore sem;    /**< semaphore used by this monitor  */
};

extern struct monent montab[];

/** Determine if a monitor is invalid or not in use  */
#define isbadmon(m) ((m >= NMON) || (MFREE == montab[m].state))

/* Monitor function prototypes */
xinu_syscall lock(monitor);
xinu_syscall unlock(monitor);
monitor moncreate(void);
xinu_syscall monfree(monitor);
xinu_syscall moncount(monitor);

#endif /* _MONITOR_H */
