/**
 * @file usbkbd.h
 *
 * Header for USB keyboard driver (supports HID boot protocol only).
 */
/* Embedded Xinu, Copyright (C) 2013, 2018.  All rights reserved. */

#ifndef _USBKBD_H_
#define _USBKBD_H_

#include <stdbool.h>
#include <xinu.h>
#include <conf.h>
#include <device.h>
#include <semaphore.h>
#include <usb_util.h>

struct usb_xfer_request;
struct usb_device;

/* Tracing macros */
//#define ENABLE_TRACE_USBKBD
#ifdef ENABLE_TRACE_USBKBD
#  include <kernel.h>
#  include <thread.h>
#  define USBKBD_TRACE(...)     { \
		kprintf("%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
		kprintf(__VA_ARGS__); \
		kprintf("\n"); }
#else
#  define USBKBD_TRACE(...)
#endif

/* usbKbdControl() requests  */
#define USBKBD_CTRL_SET_IFLAG   0x0010 /**< set input flags               */
#define USBKBD_CTRL_CLR_IFLAG   0x0011 /**< clear input flags             */
#define USBKBD_CTRL_GET_IFLAG   0x0012 /**< get input flags               */

/* USB keyboard input flags  */
#define USBKBD_IFLAG_NOBLOCK    0x0001 /**< do non-blocking input         */

/* Maximum number of input bytes to buffer  */
#define USBKBD_IBLEN 512

/** USB keyboard control block  */
struct usbkbd
{
    bool initialized;					/**< Has the keyboard structure been
                                         initialized?  */
    bool attached;						/**< Is the keyboard attached?          */
    unsigned char iflags;               /**< Input flags                        */
    semaphore isema;					/**< Count of input bytes ready         */
    unsigned short istart;              /**< Index of first byte ready          */
    unsigned short icount;              /**< Bytes in input buffer              */
    unsigned char in[USBKBD_IBLEN];     /**< Input buffer                       */
	unsigned char xfer_buf[8];			/**< Transfer buffer					*/				
    struct usb_xfer_request *intr;		/**< USB transfer request for keyboard
                                         interrupt data  */
    unsigned char recent_usage_ids[6];  /**< Keys that were reported as "down" in
                                         the last report  */
};

extern struct usbkbd usbkbds[NUSBKBD];

/* usbkbd driver functions --- only call through device entries  */
xinu_devcall usbKbdControl(device *devptr, int func, long arg1, long arg2);
xinu_devcall usbKbdGetc(device *devptr);
xinu_devcall usbKbdInit(device *devptr);
void    usbKbdInterrupt(struct usb_xfer_request *req);
xinu_devcall usbKbdRead(device *devptr, void *buf, unsigned int len);

/* usbkbd internal functions  */
usb_status_t usbKbdBindDevice(struct usb_device *dev);
void usbKbdUnbindDevice(struct usb_device *dev);

#endif  /* _USBKBD_H_ */
