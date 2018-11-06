/**
 * @file telnetGetc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <device.h>
#include <telnet.h>

/**
 * @ingroup telnet
 *
 * Read a single character from TELNET.
 * @param devptr TELNET device table entry
 * @return character read from TELNET, or EOF
 */
xinu_devcall telnetGetc(device *devptr)
{
    char ch;
    int result = NULL;

    result = telnetRead(devptr, &ch, 1);

    if (1 != result)
    {
        return result;
    }

    return ch;
}
