/**
 * @file bufpool.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#ifndef _BUFPOOL_H_
#define _BUFPOOL_H_

#include <xinu.h>
#include <semaphore.h>
#include <conf.h>

/* Buffer pool state definitions */
#define BFPFREE  0
#define BFPUSED  1

/**
 * Individual pool buffer
 */
struct poolbuf
{
    struct poolbuf *next;
    int poolid;
};

/**
 * Buffer pool table entry
 */
struct bfpentry
{
    unsigned char state;
    unsigned int bufsize;
    unsigned int nbuf;
    void *head;
    struct poolbuf *next;
    semaphore freebuf;
};

/**
 * isbadpool - check validity of reqested buffer pool id and state
 * @param p id number to test
 */
#define isbadpool(p) ((p >= NPOOL)||(p < 0)||(BFPFREE == bfptab[p].state))

/** roundword - round byte sized request to word size
 *  @param b size in bytes
 */
#define roundword(b) ((3 + b) & ~0x03)

extern struct bfpentry bfptab[];	// Defined in bpalloc

/* function prototypes */
void *bufget(int);
xinu_syscall buffree(void *);
int bfpalloc(unsigned int, unsigned int);
xinu_syscall bfpfree(int);

#ifndef NPOOL
#  define NPOOL 0
#endif

/* Fill in dummy definitions if no pools are defined  */
#if NPOOL == 0
#  ifndef POOL_MAX_BUFSIZE
#    define POOL_MAX_BUFSIZE 0
#  endif
#  ifndef POOL_MIN_BUFSIZE
#    define POOL_MIN_BUFSIZE 0
#  endif
#  ifndef POOL_MAX_NBUFS
#    define POOL_MAX_NBUFS 0
#  endif
#endif

#endif                          /* _BUFPOOL_H_ */
