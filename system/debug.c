/**
 * @file     debug.c
 * This file provides various debug utilities for development.
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <stdio.h>
#include <debug.h>

static void hexdump_print(unsigned char, unsigned char);

/**
 * Print a byte of data in mode.
 * @param data   byte of data to print
 * @param mode   ASCII or HEX
 */
static void hexdump_print(unsigned char data, unsigned char mode)
{
    switch (mode)
    {
    case DEBUG_ASCII:
        data = (' ' <= data && data <= '~') ? data : '.';
        fprintf(stdout, "%c", data);
        break;
    case DEBUG_HEX:
        fprintf(stdout, "%02x ", data);
        break;
    default:
        break;
    }
}

/**
 * @ingroup misc
 *
 * Dump a buffer of given length to stdout.
 * @param *buffer buffer to print out data in
 * @param length  length of buffer to print
 * @param canon   canonical representation (hex+ASCII)
 */
void hexdump(void *buffer, unsigned long length, bool canon)
{
    unsigned long m, n, remain;

    unsigned char *b = (unsigned char *)buffer;

    for (n = 0; n < length; n += 0x10)
    {
        fprintf(stdout, "%08lx ", (unsigned long)buffer + n);

        remain = length - n;

        for (m = 0; m < remain && m < 0x10; m++)
        {
            if (m % 0x08 == 0)
            {
                fprintf(stdout, " ");
            }
            hexdump_print(b[n + m], DEBUG_HEX);
        }

        /* pad the rest if needed */
        if (remain < 0x10)
        {
            for (m = 0; m < 0x10 - remain; m++)
            {
                if ((0 != m) && (0 == m % 0x08))
                {
                    fprintf(stdout, " ");
                }
                fprintf(stdout, "   ");
            }
        }

        if (TRUE == canon)
        {
            fprintf(stdout, " |");
            for (m = 0; m < remain && m < 0x10; m++)
            {
                hexdump_print(b[n + m], DEBUG_ASCII);
            }
            fprintf(stdout, "|");
        }

        fprintf(stdout, "\n");
    }
}
