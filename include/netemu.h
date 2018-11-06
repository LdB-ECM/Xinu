/**
 * @file netemu.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#ifndef _NETEMU_H_
#define _NETEMU_H_

#include <xinu.h>

xinu_syscall netemu(struct packet *pkt);
xinu_syscall emuCorrupt(struct packet *pkt);
xinu_syscall emuDelay(struct packet *pkt);
xinu_syscall emuDrop(struct packet *pkt);
xinu_syscall emuDuplicate(struct packet *pkt);
xinu_syscall emuReorder(struct packet *pkt);
#endif                          /* _NETEMU_H_ */
