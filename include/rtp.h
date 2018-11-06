/**
 * @file rtp.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#ifndef _RTP_H_
#define _RTP_H_

#include <xinu.h>
#include <network.h>
#include <ipv4.h>
#include <semaphore.h>
#include <stdarg.h>
#include <udp.h>

/* Tracing macros */
//#define TRACE_RTP     TTY1
#ifdef TRACE_RTP
#include <stdio.h>
#define RTP_TRACE(...)     { \
        fprintf(TRACE_RTP, "%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
        fprintf(TRACE_RTP, __VA_ARGS__); \
        fprintf(TRACE_RTP, "\n"); }
#else
#define RTP_TRACE(...)
#endif

/* RTP definitions */
#define RTP_HDR_LEN	        12      //Minimum
#define RTP_MAX_PKTS        100
#define RTP_MAX_DATALEN     1450
#define RTP_TTL             64

/* RTP standard ports */
#define RTP_PORT_RDATE          //37
#define RTP_PORT_DNS            //53
#define RTP_PORT_DHCPS          //67
#define RTP_PORT_DHCPC          //68
#define RTP_PORT_TRACEROUTE     //33434

/* RTP Control Bits */
#define RTP_VERSION		2
#define RTP_RESET		0x8000
#define RTP_PAD_YES		0x2000  //or
#define RTP_PAD_NO		0x9FFF  //and
#define RTP_EXT_YES		0x1000  //or
#define RTP_EXT_NO		0xAFFF  //and
#define RTP_CC_RESET	0xF0FF  //and
#define RTP_MARK_YES	0x0080  //or
#define RTP_MARK_NO		0xFF7F  //and

/* RTP state constants */
#define RTP_FREE      0
#define RTP_ALLOC     1
#define RTP_OPEN      2

/* Local port allocation ranges */
#define RTP_PSTART  1024   /**< start port for allocating */
#define RTP_PMAX    65535   /**< max RTP port */

/* RTCP Packet types */
#define RTCP_EP		0       /**< Encyrption prefix			  */
#define RTCP_SRRR	1       /**< Send/Receive Report		  */
#define RTCP_ARRS	2       /**< Additional RRs				  */
#define RTCP_SDES	3       /**< Source description time	  */
#define RTCP_BYEAPP	4       /**< End or App Specification     */

#ifndef __ASSEMBLER__

/*
 * RTP HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |V=2|P|X|  CC   |M|     PT      |       Sequence Number         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                           Timestamp                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |           Synchronization Source (SSRC) Identifier            |
 * +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 * |            Contributing Source (CSRC) Identifiers			   |
 * |							  ...				               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

struct rtpPkt                   /* RTP Packet Variables */
{
    unsigned short control;		/**< RTP Control Bits 0-1: Version(2)
									2: Padding 3:Extension 4-7: CSRC Count
									8: Marker 9-15: Payload Type		*/
    unsigned short seqNum;      /**< RTP Sequence Number            */
    unsigned char timestamp;    /**< RTP Timestamp                  */
    unsigned char ssrc;         /**< RTP Synchronization Source ID  */
    unsigned char csrc[1];      /**< RTP Contributing Source ID
								   (optional) (0-15 Items 32 Bits each)*/
};

/*
 * RTP Header Extension
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |      defined by profile       |           length                |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                        header extension                         |
 * |                              ...                                |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

struct rtpPktExt                /* Exists temporarily with X in header =1 */
{
    unsigned short defPro;
    unsigned short len;
    unsigned char headerExt;
};

/*
struct rtcpPkt
{
	
};
*/

/* RT Control Protocol Block */

struct rtp
{
    device *dev;                        /**< RTP device entry               */
    struct rtpPkt *in[RTP_MAX_PKTS];    /**< Pointers to stored packets     */
    int inPool;                         /**< Pool of received RTP packets   */
    int icount;                         /**< Count value for input buffer   */
    int istart;                         /**< Start value for input buffer   */
    semaphore isem;                     /**< Semaphore for input buffer     */

    unsigned short localpt;             /**< RTP Local Port					*/
    unsigned short remotept;            /**< RTP Remote Port				*/

    unsigned short clocalpt;            /**< RTCP Local Port, +1 localpt	*/
    unsigned short cremotept;           /**< RTCP Remote Port, +1 remotept	*/

    unsigned short sequence;
    unsigned char state;                /**< RTP state                      */
    unsigned char tp;
    unsigned char tc;
    unsigned char tn;
    int pmembers;
    int members;
    int senders;
    int rtcp_bw;
    unsigned char we_sent;
    unsigned char avg_rtcp_size;
    unsigned char initial;
};

extern struct rtp rtptab[];

/* Function Prototypes */
xinu_devcall rtpInit(device *);
xinu_devcall rtpOpen(device *, va_list);
xinu_devcall rtpClose(device *);
xinu_devcall rtpRead(device *, void *, unsigned int);
xinu_devcall rtpWrite(device *, void *, unsigned int);
unsigned short rtpAlloc(void);
xinu_syscall rtpRecv(struct packet *, struct netaddr *, struct netaddr *);
xinu_syscall rtpSend(struct rtp *, unsigned short, void *);
struct rtpPkt *rtpGetbuf(struct rtp *);
xinu_syscall rtpFreebuf(struct rtpPkt *);

#endif                          /* __ASSEMBLER__ */

#endif                          /* _RTP_H_ */
