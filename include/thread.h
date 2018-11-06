/**
 * @file thread.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _THREAD_H_
#define _THREAD_H_

#ifndef __ASSEMBLER__
#include <stdint.h>
#include <xinu.h>
#include <interrupt.h>
#include <semaphore.h>
#include <debug.h>
#include <memory.h>
#endif /* __ASSEMBLER__ */

/* unusual value marks the top of the thread stack                      */
#define STACKMAGIC  0x0A0AAAA9

/* miscellaneous thread definitions                                     */
#define TNMLEN      16          /**< length of thread "name"            */
#define NULLTHREAD  0           /**< id of the null thread              */
#define BADTID      (-1)        /**< used when invalid tid needed       */

/* thread initialization constants */
#ifndef INITSTK
#define INITSTK     65536       /**< initial thread stack size          */
#endif
#define INITPRIO    20          /**< initial thread priority            */
#define MINSTK      128         /**< minimum thread stack size          */
#ifdef JTAG_DEBUG
#define INITRET   debugret      /**< threads return address for debug   */
#else                           /* not JTAG_DEBUG */
#define INITRET   userret       /**< threads return address             */
#endif                          /* JTAG_DEBUG */

/* Check for invalid thread ids.  Note that interrupts must be disabled */
/* for the condition to hold true between statements.                   */
#define isbadtid(x) ((x)>=NTHREAD || (x)<0 || THRFREE == thrtab[(x)].state)

/** Maximum number of file descriptors a thread can hold */
#define NDESC       5

/** Maximum number of local devices */
#define NLOCDEV     10

/* Expose sizeof(struct thrent) and offsetof(struct thrent, stkdiv) to 
 * assembly files. */
//#define THRENTSIZE 148
//#define STKDIVOFFSET 104

#ifndef __ASSEMBLER__

/**
 * Defines what an entry in the thread table looks like.
 */
struct thrent
{        
	enum {
		THRCURR		= 1,
		THRFREE		= 2,
		THRREADY	= 3,
		THRRECV		= 4,
		THRSLEEP	= 5,
		THRSUSP		= 6,
		THRWAIT     = 7,
		THRTMOUT    = 8,
		THRMIGRATE  = 9,
	} state;						/**< thread state: THRCURR, etc.        */
    int prio;						/**< thread priority                    */
	void *stkptr;					/**< saved stack pointer                */
    void *stkbase;					/**< base of run time stack             */
	unsigned int stklen;			/**< stack length in bytes              */
    char name[TNMLEN];				/**< thread name                        */
    irqmask intmask;				/**< saved interrupt mask               */
    semaphore sem;					/**< semaphore waiting for              */
    tid_typ parent;					/**< tid for the parent thread          */
    xinu_message msg;			    /**< message sent to this thread        */
	struct {
		unsigned hasmsg : 1;		/**< nonzero iff msg is valid           */
		unsigned coreaffinity : 1;	/**< nonzero if core affinity is set	*/
		unsigned coreid : 3;		/**< core affinity for the thread       */
	};
    struct memblock memlist;		/**< free memory list of thread         */
    int fdesc[NDESC];				/**< device descriptors for thread      */
};

extern struct thrent thrtab[];
extern int thrcount;				/**< currently active threads           */
extern tid_typ thrcurrent;			/**< currently executing thread         */

/* Inter-Thread Communication prototypes */
xinu_syscall send (tid_typ, xinu_message);
xinu_message receive (void);
xinu_message recvclr (void);
xinu_message recvtime (int);

/* Thread management function prototypes */
tid_typ create(void *procaddr, unsigned int ssize, int priority,
               const char *name, int nargs, ...);
tid_typ gettid (void);
xinu_syscall getprio (tid_typ);
xinu_syscall kill (int);
int ready (tid_typ);
int resched (void);
xinu_syscall sleep (unsigned int);
xinu_syscall unsleep (tid_typ);
xinu_syscall yield (void);

/**
 * @ingroup threads
 *
 * Enter some kind of powerdown state (if it exists) that suspends
 * execution until an interrupt is detected.
 */
void pause(void);

void userret(void);

#endif /* __ASSEMBLER__ */

#endif                          /* _THREAD_H_ */
