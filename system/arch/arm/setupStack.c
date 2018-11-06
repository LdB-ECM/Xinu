/**
 * @file setupStack.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <stdint.h>
#include <platform.h>
#include <arm.h>

/* Length of ARM context record in words (includes r0-r11, cpsr, lr, pc).  */
#define CONTEXT_WORDS 15

/* The standard ARM calling convention passes first four arguments in r0-r3; the
 * rest spill onto the stack.  */
#define MAX_REG_ARGS 4

/** Set up the context record and arguments on the stack for a new thread
 * (ARM version)  */
void *setupStack(void *stackaddr, void *procaddr,
                 void *retaddr, unsigned int nargs, va_list ap)
{
    unsigned int spilled_nargs;
    unsigned int reg_nargs;
    unsigned int i;
    uint32_t* saddr = stackaddr;

    /* Determine if any arguments will spill onto the stack (outside the context
     * record).  If so, reserve space for them.  */
    if (nargs > MAX_REG_ARGS) {
        spilled_nargs = nargs - MAX_REG_ARGS;
        reg_nargs = MAX_REG_ARGS;
        saddr -= spilled_nargs;
    } else {
        spilled_nargs = 0;
        reg_nargs = nargs;
    }

    /* Possibly skip a word to ensure the stack is aligned on 8-byte boundary
     * after the new thread pops off the context record.  */
    if ((uint32_t)saddr & 0x4)
    {
        --saddr;
    }

    /* Construct the context record for the new thread.  */

    saddr -= CONTEXT_WORDS;

    /* Arguments passed in registers (part of context record)  */
    for (i = 0; i < reg_nargs; i++)
    {
        saddr[i] = va_arg(ap, unsigned long);
    }

    for (; i < CONTEXT_WORDS - 3; i++)
    {
        saddr[i] = 0;
    }

    /* Control bits of program status register
     * (SYS mode, IRQs initially enabled) */
    saddr[CONTEXT_WORDS - 3] = ARM_MODE_SYS | ARM_F_BIT;

    /* return address  */
    saddr[CONTEXT_WORDS - 2] = (uint32_t)retaddr;

    /* program counter  */
    saddr[CONTEXT_WORDS - 1] = (uint32_t)procaddr;

    /* Arguments spilled onto stack (not part of context record)  */
    for (i = 0; i < spilled_nargs; i++)
    {
        saddr[CONTEXT_WORDS + i] = va_arg(ap, unsigned long);
    }

    /* Return "top" of stack (lowest address).  */
    return saddr;
}
