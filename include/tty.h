/**
 * @file tty.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#ifndef _TTY_H_
#define _TTY_H_

#include <stdbool.h>
#include <xinu.h>
#include <device.h>
#include <stdarg.h>

#ifndef TTY_IBLEN
#define TTY_IBLEN           1024 /**< input buffer length               */
#endif

/* TTY input flags */
#define TTY_IRAW            0x01 /**< read unbuffered and uncooked      */
#define TTY_INLCR           0x02 /**< convert '\n' to '\r'              */
#define TTY_IGNCR           0x04 /**< ignore '\r' on input              */
#define TTY_ICRNL           0x08 /**< convert '\r' to '\n'              */
#define TTY_ECHO            0x10 /**< echo input                        */
#define TTY_IALL (TTY_IRAW | TTY_INLCR | TTY_IGNCR | TTY_ICRNL | TTY_ECHO)

/* TTY output flags */
#define TTY_ONLCR           0x01 /**< convert '\n' to '\r\n'            */
#define TTY_OCRNL           0x02 /**< convert '\r' to '\n'              */

/* ttyControl() functions  */
#define TTY_CTRL_SET_IFLAG  0x20 /**< set input flags                   */
#define TTY_CTRL_CLR_IFLAG  0x21 /**< clear input flags                 */
#define TTY_CTRL_SET_OFLAG  0x22 /**< set output flags                  */
#define TTY_CTRL_CLR_OFLAG  0x23 /**< clear output flags                */

/**
 * TTY control block
 */
struct tty
{
    /* Pointers to associated structures */
    device *phw;                /**< hardware device structure          */

    /* TTY input fields */
    unsigned char iflags;       /**< Input flags, TTY_I* above          */
    bool ieof;                  /**< EOF read                           */
    bool idelim;                /**< partial line in buffer             */
    char in[TTY_IBLEN];         /**< input buffer                       */
    unsigned int istart;        /**< index of first char in buffer      */
    unsigned int icount;        /**< number of characters in buffer     */

    /* TTY output fields */
    unsigned char oflags;       /**< Output flags, TTY_O* above         */
};

extern struct tty ttytab[];

/* Driver functions */
xinu_devcall ttyInit(device *);
xinu_devcall ttyOpen(device *, va_list);
xinu_devcall ttyClose(device *);
xinu_devcall ttyRead(device *, void *, unsigned int);
xinu_devcall ttyWrite(device *, const void *, unsigned int);
xinu_devcall ttyGetc(device *);
xinu_devcall ttyPutc(device *, char);
xinu_devcall ttyControl(device *, int, long, long);

#endif                          /* _TTY_H_ */
