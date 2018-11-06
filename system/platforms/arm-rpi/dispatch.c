/**
 * @file dispatch.c
 *
 * This file contains code that interfaces with the interrupt controller of the
 * BCM2835 SoC used in the Raspberry Pi.
 *
 * This "interrupt controller" is, more specifically, the interrupt controller
 * used by the ARM processor on the BCM2835.  That is, this interrupt controller
 * controls which IRQs the ARM receives.  The VideoCore co-processor likely has
 * its own (undocumented) way to control IRQs to itself.
 *
 * See http://xinu-os.org/BCM2835_Interrupt_Controller for additional
 * information.
 */
/* Embedded Xinu, Copyright (C) 2013, 2018.  All rights reserved. */

#include <stdbool.h>
#include <stdint.h>
#include <kernel.h>
#include <platform.h>
#include <clock.h>
#include "interrupt.h"
#include "rpi-platform.h"

/*==========================================================================}
{  RASPBERRY PI SYSTEM TIMER HARDWARE REGISTERS - BCM2835 Manual Section 12	}
{--------------------------------------------------------------------------*/
/* The packing and alignment required for 64 bit compiler and not optional  }
{==========================================================================*/
struct __attribute__((__packed__, aligned(4))) bcm2835_interrupt_registers 
{
    uint32_t IRQ_basic_pending;
    uint32_t IRQ_pending_1;
    uint32_t IRQ_pending_2;
    uint32_t FIQ_control;
    uint32_t Enable_IRQs_1;
    uint32_t Enable_IRQs_2;
    uint32_t Enable_Basic_IRQs;
    uint32_t Disable_IRQs_1;
    uint32_t Disable_IRQs_2;
    uint32_t Disable_Basic_IRQs;
};

/*--------------------------------------------------------------------------}
;{			RASPBERRY PI INTERRUPT CONTROLLER  REGISTER LOCATIONS			}
;{-------------------------------------------------------------------------*/
#define IRQ_CONTROL ((volatile __attribute__((aligned(4))) struct bcm2835_interrupt_registers*)(RPi_IO_Base_Addr + 0xB200))

/* Number of IRQs shared between the GPU and ARM. These correspond to the IRQs
 * that show up in the IRQ_pending_1 and IRQ_pending_2 registers.  */
#define BCM2835_NUM_GPU_SHARED_IRQS     64

/* Number of ARM-specific IRQs. These correspond to IRQs that show up in the
 * first 8 bits of IRQ_basic_pending.  */
#define BCM2835_NUM_ARM_SPECIFIC_IRQS   8

/* Total number of IRQs on this hardware ... the value is 72 .. (64 + 8)  */
#define BCM2835_NUM_IRQS (BCM2835_NUM_GPU_SHARED_IRQS + BCM2835_NUM_ARM_SPECIFIC_IRQS)

/** Table of Xinu's interrupt handler functions.  This is an array mapping IRQ
 * numbers to handler functions.  They all start as NULL */
interrupt_handler_t interruptVector[BCM2835_NUM_IRQS] = { 0 };

/** Bitwise table of IRQs that have been enabled on the ARM. They all start disabled */
static uint32_t arm_enabled_irqs[3] = { 0 };

/* Call the handler function for an IRQ that was received, or panic if it
 * doesn't exist.  */
static void handle_irq (uint8_t irq_num)
{
    interrupt_handler_t handler = interruptVector[irq_num];
    if (handler)
    {
        (*handler)();
    }
    else
    {
        kprintf("ERROR: No handler registered for interrupt %u\r\n", irq_num);

        extern void halt(void);
        halt();
    }
}

/* Check if the pending bit for an IRQ line is set; if so, call the handler
 * function.  */
static void check_irq_pending (uint8_t irq_num)
{
    bool handle = FALSE;

    /* Check the appropriate hardware register, depending on the IRQ number.  */
    if (irq_num >= 64)
    {
        if (IRQ_CONTROL->IRQ_basic_pending & (1 << (irq_num - 64)))
        {
            handle = TRUE;
        }
    }
    else if (irq_num >= 32)
    {
        if (IRQ_CONTROL->IRQ_pending_2 & (1 << (irq_num - 32)))
        {
            handle = TRUE;
        }
    }
    else
    {
        if (IRQ_CONTROL->IRQ_pending_1 & (1 << irq_num))
        {
            handle = TRUE;
        }
    }
    if (handle)
    {
        handle_irq(irq_num);
        /* The pending bit should have been cleared in a device-specific way by
         * the handler function.  As far as we can tell, it cannot be cleared
         * directly through the interrupt controller.  */
    }
}

/* Find index of first set bit in a nonzero word. */
static inline unsigned int first_set_bit (uint32_t word)
{
    return 31 - __builtin_clz(word);
}

/**
 * Processes all pending interrupt requests.
 *
 * On the BCM2835 (Raspberry Pi), this done by iterating through all registered
 * interrupts on the ARM and checking whether each one is pending.  This is not
 * necessarily the fastest way to do it, but this should minimize problems with
 * the poorly-documented hardware and conflicts with the GPU.
 */
void dispatch (void)
{
    for (int i = 0; i < 3; i++)
    {
        uint32_t mask = arm_enabled_irqs[i];
        while (mask != 0)
        {
            unsigned int bit = first_set_bit(mask);
            mask ^= (1 << bit);
            check_irq_pending(bit + (i << 5));
        }
    }
}

/**
 * Enable an interrupt request line.
 * @param irq_num
 *      index of the interrupt to enable, which must be valid on the current
 *      platform.
 */
void enable_irq (irqmask irq_num)
{
    if (irq_num < 32)
    {
		IRQ_CONTROL->Enable_IRQs_1 = 1 << irq_num;
        arm_enabled_irqs[0] |= 1 << irq_num;
    }
    else if (irq_num < 64)
    {
		IRQ_CONTROL->Enable_IRQs_2 = 1 << (irq_num - 32);
        arm_enabled_irqs[1] |= 1 << (irq_num - 32);
    }
    else
    {
		IRQ_CONTROL->Enable_Basic_IRQs = 1 << (irq_num - 64);
        arm_enabled_irqs[2] |= 1 << (irq_num - 64);
    }
}

/**
 * Disable an interrupt request line.
 * @param irq_num
 *      index of the interrupt to disable, which must be valid on the current
 *      platform.
 */
void disable_irq (irqmask irq_num)
{
    if (irq_num < 32)
    {
		IRQ_CONTROL->Disable_IRQs_1 = 1 << irq_num;
        arm_enabled_irqs[0] &= ~(1 << irq_num);
    }
    else if (irq_num < 64)
    {
		IRQ_CONTROL->Disable_IRQs_2 = 1 << (irq_num - 32);
        arm_enabled_irqs[1] &= ~(1 << (irq_num - 32));
    }
    else
    {
		IRQ_CONTROL->Disable_Basic_IRQs = 1 << (irq_num - 64);
        arm_enabled_irqs[2] &= ~(1 << (irq_num - 64));
    }
}


int set_interrupt_handler(unsigned int intnum, interrupt_handler_t handler)
{
	if (intnum >= BCM2835_NUM_IRQS) return SYSERR;
	interruptVector[intnum] = handler;
	return OK;
}

#if RTCLOCK
/** @ingroup timer
 *
 * Number of timer interrupts that have occurred since ::clktime was
 * incremented.  When ::clkticks reaches ::CLKTICKS_PER_SEC, ::clktime is
 * incremented again and ::clkticks is reset to 0.
 */
volatile unsigned long clkticks;

/** @ingroup timer
 * Number of seconds that have elapsed since the system booted.  */
volatile unsigned long clktime;

/** Queue of sleeping processes.  */
qid_typ sleepq;

/**
 * @ingroup timer
 *
 * Initialize the clock and sleep queue.  This function is called at startup.
 */
void clkinit(void)
{
	sleepq = queinit();         /* initialize sleep queue       */

	clkticks = 0;

#ifdef DETAIL
	kprintf("Time base %dHz, Clock ticks at %dHz\r\n",
		platform.clkfreq, CLKTICKS_PER_SEC);
#endif
	/* register clock interrupt */
	interruptVector[IRQ_TIMER] = clkhandler;
	enable_irq(IRQ_TIMER);
	clkupdate(platform.clkfreq / CLKTICKS_PER_SEC);
}

#endif                          /* RTCLOCK */