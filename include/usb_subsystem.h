/**
 * @file usb_subsystem.h
 * @ingroup usb
 *
 * Interface to the USB (Universal Serial Bus) subsystem from other operating
 * system code.
 */
/* Embedded Xinu, Copyright (C) 2013,2018.  All rights reserved. */

#ifndef _USB_SUBSSYSTEM_H_
#define _USB_SUBSSYSTEM_H_

#include <xinu.h>

xinu_syscall usbinit(void);
xinu_syscall usbinfo(void);

/* Must be provided by platform implementation in say platforminit.c*/
xinu_syscall usbpoweron(void);
xinu_syscall usbpoweroff(void);

#endif /* _USB_SUBSSYSTEM_H_ */
