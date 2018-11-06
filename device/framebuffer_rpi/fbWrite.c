/**
 * @file fbWrite.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <framebuffer.h>

/**
 * @ingroup framebuffer
 *
 * Write a buffer of characters to the framebuffer.
 *
 * @param devptr  pointer to framebuffer device
 * @param buf   buffer of characters to write
 * @param len   number of characters to write from the buffer
 *
 * @return
 *      The number of characters written, which may be less than @p len in the
 *      event of a write error; or ::SYSERR if an error occurred before any
 *      characters at all were written.
 */
xinu_devcall fbWrite (device *devptr, const void* buf, unsigned int len)
{
    unsigned int count;
    int result;

    for (count = 0; count < len; count++)
    {
		/* Next byte to write.  */
		unsigned char ch = ((const unsigned char *)buf)[count];

        result = fbPutc(devptr, ch);
        if (result != ch)
        {
            if (count == 0)
            {
                return SYSERR;
            }
            break;
        }
    }
    return count;
}
