/**
 * @file insertd.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <thread.h>
#include <queue.h>

/**
 * @ingroup threads
 *
 * Insert a thread in delta queue in ascending order
 * @param tid    thread id to insert
 * @param q      queue in which thread should be inserted
 * @param key    delta key
 * @return OK
 */
int insertd (tid_typ tid, qid_typ q, int key)
{
    int next;                   /* runs through list                  */
    int prev;                   /* follows next through list          */

    if (isbadqid(q) || isbadtid(tid))
    {
        return SYSERR;
    }

    prev = quehead(q);
    next = quetab[quehead(q)].next;
    while ((quetab[next].key <= key) && (next != quetail(q)))
    {
        key -= quetab[next].key;
        prev = next;
        next = quetab[next].next;
    }
    quetab[tid].next = next;
    quetab[tid].prev = prev;
    quetab[tid].key = key;
    quetab[prev].next = tid;
    quetab[next].prev = tid;
    if (next != quetail(q))
    {
        quetab[next].key -= key;
    }

    return OK;
}
