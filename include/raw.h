/**
 * @file raw.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _RAW_H_
#define _RAW_H_

#include <xinu.h>
#include <ipv4.h>
#include <network.h>
#include <semaphore.h>
#include <stdarg.h>

/* Tracing macros */
//#define TRACE_RAW     TTY1
#ifdef TRACE_RAW
#include <stdio.h>
#define RAW_TRACE(...)     { \
		fprintf(TRACE_RAW, "%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
		fprintf(TRACE_RAW, __VA_ARGS__); \
		fprintf(TRACE_RAW, "\n"); }
#else
#define RAW_TRACE(...)
#endif


/* State */
#define RAW_FREE        0
#define RAW_ALLOC       1

/* Buffer lengths */
#define RAW_IBLEN       100

/* Input flags */
#define RAW_IACCEPT     0x01    /**< Set remoteip using next read pkt  */
#define RAW_IHDR        0x02    /**< Include network layer header      */

/* Output flags */
#define RAW_OHDR        0x04    /**< Pkt already has network layer hdr */

/* Control functions */
#define RAW_CTRL_SETFLAG   1    /**< Set flags                         */
#define RAW_CTRL_CLRFLAG   2    /**< Clear flags                       */

/**
 *  Raw socket control block 
 */
struct raw
{
    /* Pointers to associated structures */
    device *dev;                    /**< Dev structure                      */
    unsigned char state;            /**< Device state                       */

    /* Connection fields */
    unsigned short proto;           /**< IP protocol to accept              */
    struct netaddr localip;         /**< Local IP address                   */
    struct netaddr remoteip;        /**< Remote IP address                  */

    /* Input fields */
    struct packet *in[RAW_IBLEN];        /**< Input buffer                  */
    struct netaddr src[RAW_IBLEN];       /**< Remoteip buffer               */
    int icount;                     /**< Packets in buffer                  */
    int istart;                     /**< Index of first packet              */
    semaphore isema;                /**< Count of input packets ready       */

    unsigned char flags;            /**< Flags                              */
};

extern struct raw rawtab[];

/* Function Prototypes */
xinu_devcall rawClose(device *pdev);
xinu_devcall rawControl(device *pdev, int func, long arg1, long arg2);
struct raw *rawDemux(struct netaddr *src, struct netaddr *dst,
                     unsigned short proto);
xinu_devcall rawInit(device *pdev);
xinu_devcall rawOpen(device *pdev, va_list ap);
xinu_devcall rawRead(device *pdev, void *buf, unsigned int len);
xinu_syscall rawRecv(struct packet *pkt, struct netaddr *src,
                     struct netaddr *dst, unsigned short proto);
xinu_syscall rawSend(struct raw *rawptr, const void *buf, unsigned int len);
xinu_devcall rawWrite (device *pdev, const void *buf, unsigned int len);

#endif                          /* _RAW_H_ */
