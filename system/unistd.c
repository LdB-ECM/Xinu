/**
 * @file unistd.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <stdio.h>
#include <stdarg.h>
#include <tty.h>
#include <ctype.h>

xinu_syscall getnum(void)
{
    int i = 0, c = 0;

    control(CONSOLE, TTY_CTRL_SET_IFLAG, TTY_ECHO, 0);
    fprintf(CONSOLE, "? ");

    c = getchar();
    while (('\n' != c) && ('\r' != c) && (!iseof(c)))
    {
        if (('0' <= c) && ('9' >= c))
        {
            i = i * 10 + c - '0';
        }

        c = getchar();
    }

    if (EOF == c)
        return c;

    return i;
}

xinu_syscall printnum (int i)
{
    return fprintf(CONSOLE, "%d\n", i);
}
