/**
* @file xinu.h
*/
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

/* This is constants and proptypes that are specific to the xinu system  */
/* In earlier Xinu versions they were in system files like stddef.h in   */
/* violation of the C standard library headers. The aim was to move them */
/* here and allow Xinu to be at least C11 compliant                     */

#ifndef _XINU_H_
#define _XINU_H_

typedef int thread;													/**< thread declaration               */
typedef void interrupt;												/**< interrupt procedure		      */

typedef int xinu_syscall;												/**< xinu system call declaration     */
typedef int xinu_devcall;												/**< xinu device call declaration     */
typedef void xinu_exchandler;												/**< exception procedure              */
typedef int shellcmd;				 									/**< xinu shell command declaration   */
typedef int xinu_message;												/**< xinu message passing content     */

/* Original Universal return constants */
#define OK        1													// Xinu result id for ok meaning success
#define SYSERR   (-1)												// Xinu result id for meaning failed

typedef int tid_typ;												/**< thread ID type                     */


typedef int HANDLE;


#endif                          /* _XINU_H_ */