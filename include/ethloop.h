/**
 * @file ethloop.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#ifndef _ETHLOOP_H_
#define _ETHLOOP_H_

#include <xinu.h>
#include <device.h>
#include <ethernet.h>
#include <semaphore.h>

#define ELOOP_MTU          1500
#define ELOOP_LINKHDRSIZE  ETH_HDR_LEN

#define ELOOP_BUFSIZE      ELOOP_MTU + ELOOP_LINKHDRSIZE
#define ELOOP_NBUF         100

#define ELOOP_CTRL_GETHOLD	1
#define ELOOP_CTRL_SETFLAG  2
#define ELOOP_CTRL_CLRFLAG	3

#define ELOOP_FLAG_HOLDNXT	0x01  /**< place next written pkt in hold  */
#define ELOOP_FLAG_DROPNXT	0x04  /**< drop next written pkt           */
#define ELOOP_FLAG_DROPALL	0x08  /**< drop all written pkts           */

#define ELOOP_STATE_FREE        0
#define ELOOP_STATE_ALLOC       1

/**
 * Loopback device control block
 */
struct ethloop
{
    int state;                      /**< device state                       */
    device *dev;                    /**< device table entry                 */
    int poolid;                     /**< poolid for the buffer pool         */
    unsigned char flags;            /**< flags                              */

    /* Packet queue */
    int index;						/**< index of first packet in buffer    */
    semaphore sem;					/**< number of packets in buffer        */
    int count;                      /**< number of packets in buffer        */
    char *buffer[ELOOP_NBUF];		/**< input buffer                       */
    int pktlen[ELOOP_NBUF];         /**< length of packet in buffer         */

    /* Hold packet */
    semaphore hsem;                 /**< number of held packets             */
    char *hold;					    /**< hold buffer                        */
    int holdlen;                    /**< length of packet in hold buffer    */

    /* Statistics */
    unsigned int nout;              /**< number of packets written          */
};

extern struct ethloop elooptab[];

/* Driver functions */
xinu_devcall ethloopInit (device *);
xinu_devcall ethloopOpen (device *, va_list ap);
xinu_devcall ethloopClose (device *);
xinu_devcall ethloopRead (device *, void*, unsigned int);
xinu_devcall ethloopWrite (device *, const void *, unsigned int);
xinu_devcall ethloopControl (device *, int, long, long);

#endif                          /* _ETHLOOP_H_ */
