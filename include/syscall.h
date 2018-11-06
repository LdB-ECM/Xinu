/**
 * @file syscall.h
 * System call interface and handlers for user to kernel switches.
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <xinu.h>
#include <mailbox.h>

xinu_exchandler syscall_entry(void);
int syscall_dispatch(int, int *);

struct syscall_info
{
    int nargs;							/**< Number of arguments required    */
    xinu_syscall (*handler) (int *);    /**< Function handler                */
};

#define SYSCALL_NONE        0 /**< Perform no operation             */
#define SYSCALL_YIELD       1 /**< Yield processor                  */
#define SYSCALL_SLEEP       2 /**< Sleep for number of microseconds */
#define SYSCALL_KILL        3 /**< Kill a process                   */
#define SYSCALL_OPEN        4 /**< Open a device                    */
#define SYSCALL_CLOSE       5 /**< Close a device                   */
#define SYSCALL_READ        6 /**< Read from a device               */
#define SYSCALL_WRITE       7 /**< Write to a device                */
#define SYSCALL_GETC        8 /**< Get character from device        */
#define SYSCALL_PUTC        9 /**< Put character to device          */
#define SYSCALL_SEEK       10 /**< Seek to position on device       */
#define SYSCALL_CONTROL    11 /**< Control a device                 */
#define SYSCALL_GETDEV     12 /**< Get a device number from string  */
#define SYSCALL_MBOXALLOC  13 /**< Allocate a mailbox               */
#define SYSCALL_MBOXFREE   14 /**< Free a mailbox                   */
#define SYSCALL_MBOXRECV   15 /**< Receive message from mailbox     */
#define SYSCALL_MBOXSEND   16 /**< Send message to mailbox          */

extern const struct syscall_info syscall_table[];
extern int nsyscalls;

/* Prototypes from user mode system calls */
xinu_syscall user_none(void);
xinu_syscall user_yield(void);
xinu_syscall user_sleep(unsigned int ms);
xinu_syscall user_kill(tid_typ thread_id);
xinu_syscall user_open(int descrp, int arg);
xinu_syscall user_close(int descrp);
xinu_syscall user_read(int descrp, void *buffer, unsigned int length);
xinu_syscall user_write(int descrp, void *buffer, unsigned int length);
xinu_syscall user_getc(int descrp);
xinu_syscall user_putc(int descrp, char character);
xinu_syscall user_seek(int descrp, unsigned int offset);
xinu_syscall user_control(int descrp, int function, long arg1, long arg2);
xinu_syscall user_getdev(char *device_name);
xinu_syscall user_mboxalloc(unsigned int nmsgs);
xinu_syscall user_mboxfree(mailbox mbox_id);
xinu_syscall user_mboxrecv(mailbox mbox_id);
xinu_syscall user_mboxsend(mailbox mbox_id, int msg);

#endif                          /* __SYSCALL_H__ */
