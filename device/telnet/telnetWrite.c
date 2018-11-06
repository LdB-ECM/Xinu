/**
 * @file     telnetWrite.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <ctype.h>
#include <device.h>
#include <telnet.h>
#include <thread.h>
#include <stdio.h>

/**
 * @ingroup telnet
 *
 * Write a buffer to a telnet client
 * @param devptr TELNET device table entry
 * @param buf buffer of characters to output
 * @param len size of the buffer
 * @return count of characters output
 */
xinu_devcall telnetWrite(device *devptr, const void *buf, unsigned int len)
{
    struct telnet *tntptr;
    device *phw;
    unsigned char ch = 0;
    unsigned int count = 0;
    unsigned char *buffer = (unsigned char*)buf;

    /* Setup and error check pointers to structures */
    tntptr = &telnettab[devptr->minor];
    phw = tntptr->phw;
    if (NULL == phw)
    {
        return SYSERR;
    }

    wait(tntptr->osem);

    /* propery format and write all characters to buffer */
    while (count < len)
    {
        ch = buffer[count++];

        /* write buffer to underlying device if 2 more chars can't fit */
        if (tntptr->ostart >= TELNET_OBLEN - 1)
        {
            if (SYSERR == telnetFlush(devptr))
                return SYSERR;
        }

        switch (ch)
        {
            /* append CRLF to buffer, write buffer */
        case '\n':
            tntptr->out[tntptr->ostart++] = '\r';
            tntptr->out[tntptr->ostart++] = '\n';

            if (SYSERR == telnetFlush(devptr))
            {
                return SYSERR;
            }
            break;
            /* Escape IAC character */
        case TELNET_IAC:
            tntptr->out[tntptr->ostart++] = ch;
            tntptr->out[tntptr->ostart++] = ch;
            break;
            /* write normally */
        default:
            tntptr->out[tntptr->ostart++] = ch;
            break;
        }
    }

    signal(tntptr->osem);

    return count;
}
