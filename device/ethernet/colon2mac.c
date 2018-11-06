/**
 * @file colon2mac.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <ether.h>

#include <ctype.h>

/**
 * @ingroup ether
 *
 * Convert a colon-separated string representation of a MAC into
 *  the equivalent byte array.
 * @param src pointer to colon-separated MAC string
 * @param dst pointer to byte array
 * @return number of octets converted.
 */
int colon2mac(char *src, unsigned char *dst)
{
    unsigned char count = 0, digit = 0, c = 0;

    if (NULL == src || NULL == dst)
    {
        return SYSERR;
    }

    while ((count < ETH_ADDR_LEN) && ('\0' != *src))
    {
        c = *src++;
        if (isdigit(c))
        {
            digit = c - '0';
        }
        else if (isxdigit(c))
        {
            digit = 10 + c - (isupper(c) ? 'A' : 'a');
        }
        else
        {
            digit = 0;
        }
        dst[count] = digit * 16;

        c = *src++;
        if (isdigit(c))
        {
            digit = c - '0';
        }
        else if (isxdigit(c))
        {
            digit = 10 + c - (isupper(c) ? 'A' : 'a');
        }
        else
        {
            digit = 0;
        }
        dst[count] += digit;

        count++;
        if (':' != *src++)
        {
            break;
        }
    }

    return count;
}
