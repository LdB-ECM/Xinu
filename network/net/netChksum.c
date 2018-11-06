/**
 * @file netChksum.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <network.h>

/**
 * @ingroup network
 */
unsigned short netChksum(void *data, unsigned int len)
{
    unsigned int sum;
    unsigned short *ptr;

    /* Add data to sum */
    ptr = (unsigned short *)data;
    sum = 0;
    while (len > 1)
    {
        sum += *ptr;
        ptr++;
        len -= 2;
    }

    /* Add left-over byte, if any */
    if (len > 0)
    {
        sum += net2hs(*((unsigned char *)ptr) << 8);
    }

    /* Fold 32-bit sum into 16 bits */
    while (sum >> 16)
    {
        sum = (sum >> 16) + (sum & 0xFFFF);
    }

    return (~sum);
}
