/**
 * @file mj.c
 * For details see http://dx.doi.org/10.1145/1869542.1869568
 */
/* Embedded Xinu, Copyright (C) 2008, 2018.  All rights reserved. */

#include <xinu.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <device.h>
#include <tty.h>
#include <memory.h>
#include <thread.h>
#include <monitor.h>

xinu_syscall _readint(void)
{
    int i = 0, c = 0;

    control(CONSOLE, TTY_CTRL_SET_IFLAG, TTY_ECHO, 0);

    c = getchar();
    while (('\n' != c) && ('\r' != c) && (EOF != c))
    {
        if (('0' <= c) && ('9' >= c))
        {
            i = i * 10 + c - '0';
        }

        c = getchar();
    }
    kprintf("\r\n");
    if (EOF == c)
        return c;

    return i;
}

xinu_syscall _printint(int i)
{
    return kprintf("%d\r\n", i);
}

xinu_syscall _print(char *s)
{
    return kprintf("%s", s);
}

xinu_syscall _println(void)
{
    return kprintf("\r\n");
}

xinu_syscall _yield(void)
{
    return yield();
}

xinu_syscall _sleep(int time)
{
    return sleep(time);
}

int *_new(int n, int init)
{
    int size = (n + 2) * 4;
    int *p = (int *)memget(size);
    bzero(p, size);
    p[0] = moncreate();
    p[1] = init;
    return p + 2;
}

xinu_syscall _lock(int *objAdder)
{
    int *A = objAdder - 2;
    monitor m = (monitor) * A;
    return lock(m);
}

xinu_syscall _unlock(int *objAdder)
{
    int *A = objAdder - 2;
    monitor m = (monitor) * A;
    return unlock(m);
}

xinu_syscall _threadCreate(int *threadObjAdder)
{
    int *A = threadObjAdder - 1;
    int *B = (int *)*A;
    int *C = (int *)*B;
    void *procadder = (void *)C;
    return
        ready(create
              (procadder, INITSTK, INITPRIO, "MiniJavaThread", 1,
               threadObjAdder));
}


void _BADPTR(void)
{
    fprintf(CONSOLE, "FATAL ERROR: Null Pointer Exception!\n");
    kill(thrcurrent);
}

void _BADSUB(void)
{
    fprintf(CONSOLE, "FATAL ERROR: Index Out Of Bounds Exception!\n");
    kill(thrcurrent);
}
