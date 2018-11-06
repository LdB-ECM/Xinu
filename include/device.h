/**
 * @file device.h 
 *
 * Contains all definitions relating to the Xinu device subsystem.
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

/* Device table declarations */
#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <xinu.h>
#include <conf.h>

/**
 * Check that device id is between 0 and NDEVS. 
 *
 * @param f id number to test
 */
#define isbaddev(f)  ( !(0 <= (f) && (f) < NDEVS) )

/* Standard driver functions */
xinu_devcall open (int, ...);
xinu_devcall close (int);
xinu_devcall read (int, void *, unsigned int);
xinu_devcall write (int, const void *, unsigned int);
xinu_devcall getc (int);
xinu_devcall putc (int, char);
xinu_devcall seek (int, unsigned int);
xinu_devcall control(int, int, long, long);
xinu_syscall getdev(const char *);

#endif                          /* _DEVICE_H_ */
