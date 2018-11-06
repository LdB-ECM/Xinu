/**
 * @file kprintf.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <string.h>
#include <xinu.h>
#include <kernel.h>
#include <stdarg.h>
#include <stdio.h>
#include <conf.h>
#include <CriticalSection.h>

bool kprint_enable = false;

/**
 * @ingroup uartgeneric
 *
 * kernel printf: formatted, synchronous output to SERIAL0.
 *
 * @param format
 *      The format string.  Not all standard format specifiers are supported by
 *      this implementation.  See _doprnt() for a description of supported
 *      conversion specifications.
 * @param ...
 *      Arguments matching those in the format string.
 *
 * @return
 *      The number of characters written.
 */
xinu_syscall kprintf(const char *format, ...)
{
	int retval = OK;
	va_list ap;
	if (kprint_enable)
	{
		/* Note: it isn't strictly necessary to disable global interrupts here, but
		* it prevents kprintf()'s from stepping on each other toes if you happen to
		* call kprintf() from an interrupt handler.  */
		ENTER_KERNEL_CRITICAL_SECTION();

		va_start(ap, format);
		retval = _doprnt(format, ap, fputc, kernel_out);
		va_end(ap);

		EXIT_KERNEL_CRITICAL_SECTION();
	}
	return retval;
}

