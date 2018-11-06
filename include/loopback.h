/**
 * @file loopback.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#ifndef _LOOPBACK_H_
#define _LOOPBACK_H_

#include <stdint.h>
#include <stdarg.h>
#include <xinu.h>
#include <device.h>
#include <semaphore.h>

#ifndef LOOP_BUFFER
#define LOOP_BUFFER 1024        /**< loopback buffer length             */
#endif

/* LOOP device states */
#define LOOP_STATE_FREE     0
#define LOOP_STATE_ALLOC    1

/* loopbackControl() functions  */
#define LOOP_CTRL_SET_FLAG  0x01 /**< set flags                         */
#define LOOP_CTRL_CLR_FLAG  0x02 /**< clear flags                       */

/* loopback flags */
#define LOOP_NONBLOCK       0x01 /**< Reading and writing do not block  */

/**
 * Loopback device control block
 */
struct loopback
{
    int state;							/**< LOOP_STATE_*above                  */
    int index;							/**< index of first char in buffer      */
    int flags;							/**< loopback control flags             */
    semaphore sem;						/**< number of characters in buffer     */
    uint8_t buffer[LOOP_BUFFER];		/**< input buffer                       */
};

extern struct loopback looptab[];

/* Driver functions */
xinu_devcall loopbackInit (device *);
xinu_devcall loopbackOpen (device *, va_list ap);
xinu_devcall loopbackClose (device *);
xinu_devcall loopbackRead (device*, void*, unsigned int);
xinu_devcall loopbackWrite (device*, const void*, unsigned int);
xinu_devcall loopbackGetc (device*);
xinu_devcall loopbackPutc (device*, char);
xinu_devcall loopbackControl (device*, int, long, long);

#endif                          /* _LOOPBACK_H_ */
