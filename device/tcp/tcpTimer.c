/**
 * @file tcpTimer.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2018.  All rights reserved. */

#include <xinu.h>
#include <clock.h>
#include <CriticalSection.h>
#include <semaphore.h>
#include <stdlib.h>
#include <tcp.h>
#include <thread.h>

struct tcpEvent tcptimertab[TCP_NEVENTS];
semaphore tcpmutex;

static int calcElapsed(int, int);

/**
 * @ingroup tcp
 *
 * TCP timer process to manage timeout and retransmit events.
 */
thread tcpTimer(void)
{
    struct tcpEvent *first = NULL;
    struct tcpEvent *head = NULL;
    unsigned long lastticks;
    unsigned long lasttime;
    unsigned long elapse;
    unsigned char type;
    struct tcb *tcbptr = NULL;

    /* Setup timer event delta queue */
    bzero(tcptimertab, sizeof(struct tcpEvent) * TCP_NEVENTS);
    tcpmutex = semcreate(1);
    head = &tcptimertab[TCP_EVT_HEAD];
    head->used = TRUE;
    head->next = NULL;

    TCP_TRACE("Timer init complete");
    /* Initialize timer accounting information */
	ENTER_KERNEL_CRITICAL_SECTION();
    lastticks = clkticks;
    lasttime = clktime;
    elapse = 0;
	EXIT_KERNEL_CRITICAL_SECTION();
    sleep(TCP_FREQ);

    while (TRUE)
    {
//              TCP_TRACE("Tick");
        wait(tcpmutex);
        while ((head->next != NULL) && (elapse > 0))
        {
            first = head->next;
            if (first->remain > elapse)
            {
                first->remain -= elapse;
                break;
            }

            elapse -= first->remain;
            first->remain = 0;
            while ((first != NULL) && (first->remain <= 0))
            {
                /* Save event information and update pointers */
                type = first->type;
                tcbptr = first->tcbptr;
                first->used = FALSE;
                head->next = first->next;

                /* Release mutex in case triggered event needs it */
                signal(tcpmutex);

                /* Trigger event */
                tcpTimerTrigger(type, tcbptr);

                /* Reclaim mutex */
                wait(tcpmutex);

                /* Obtain first event (which may have changed while 
                 * mutex was released) */
                first = head->next;
            }
        }
        signal(tcpmutex);

		ENTER_KERNEL_CRITICAL_SECTION();
        elapse = calcElapsed(lastticks, lasttime);
		EXIT_KERNEL_CRITICAL_SECTION();

//        sleep(TCP_FREQ - elapse);
        sleep(TCP_FREQ);

		ENTER_KERNEL_CRITICAL_SECTION();
        elapse = calcElapsed(lastticks, lasttime);
        lastticks = clkticks;
        lasttime = clktime;
		EXIT_KERNEL_CRITICAL_SECTION();
    }
    return OK;
}

/**
 * Calulates the number of milliseconds elapsed.
 * @param lastticks last value of clock ticks
 * @param lasttime last value of clock time
 * @return number of milliseconds elapsed between the time provided and now 
 * @precondition interrupts are disabled
 * @postcondition interrupts are still disabled
 */
static int calcElapsed(int lastticks, int lasttime)
{
    int ticks = 0;

    if (lasttime == clktime)
    {
        ticks = clkticks - lastticks;
    }
    else
    {
        ticks = CLKTICKS_PER_SEC - lastticks;
        ticks += (clktime - lasttime - 1) * CLKTICKS_PER_SEC;
        ticks += clkticks;
    }

    return (ticks * (CLKTICKS_PER_SEC / 1000));
}
