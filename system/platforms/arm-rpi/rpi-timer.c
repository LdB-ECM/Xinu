/**
 * @file timer.c
 *
 * This file contains code that interfaces with the system timer of the BCM2835
 * SoC used in the Raspberry Pi.
 *
 * See 12. "System Timer" of the BCM2835-ARM-Peripherals.pdf document for
 * details about this hardware.  However, this document does not say the
 * frequency the timer runs at (it's 1 MHz), nor does it say that some of the
 * output compare registers (0 and 2) are used by the GPU and therefore cannot
 * be used by ARM code, nor does it say what interrupt line the system timer is
 * connected to (it's IRQs 0-3 mapped linearly onto the output compare
 * registers).
 *
 * See http://xinu-os.org/BCM2835_System_Timer for more information about this
 * hardware.
 */
 /* Embedded Xinu, Copyright (C) 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <stdint.h>
#include <clock.h>
#include "rpi-platform.h"

/*==========================================================================}
{  RASPBERRY PI SYSTEM TIMER HARDWARE REGISTERS - BCM2835 Manual Section 12	}
{--------------------------------------------------------------------------*/
/* The packing and alignment required for 64 bit compiler and not optional  }
{==========================================================================*/
struct __attribute__((__packed__, aligned(4))) bcm2835_timer_regs
{
	uint32_t CS;		/** System Timer Control/Status							*/
	uint32_t CLO;		/** System Timer Counter Lower 32 bits					*/
	uint32_t CHI;		/** System Timer Counter Higher 32 bits					*/
	uint32_t C0;		/** System Timer Compare 0.  DO NOT USE; is used by GPU	*/
	uint32_t C1;		/** System Timer Compare 1								*/
	uint32_t C2;		/** System Timer Compare 2.  DO NOT USE; is used by GPU	*/
	uint32_t C3;		/** System Timer Compare 3								*/
};

/*--------------------------------------------------------------------------}
;{			  RASPBERRY PI SYSTEM TIMER REGISTER LOCATIONS					}
;{-------------------------------------------------------------------------*/
#define SYSTEM_TIMER ((volatile __attribute__((aligned(4))) struct bcm2835_timer_regs*)(RPi_IO_Base_Addr + 0x3000))


#define BCM2835_SYSTEM_TIMER_MATCH_0 (1 << 0)
#define BCM2835_SYSTEM_TIMER_MATCH_1 (1 << 1)
#define BCM2835_SYSTEM_TIMER_MATCH_2 (1 << 2)
#define BCM2835_SYSTEM_TIMER_MATCH_3 (1 << 3)

/* clkcount() interface is documented in clock.h  */
/**
 * @detail
 *
 * Raspberry-Pi specific note:
 *    This function returns the BCM2835's 1Mhz free-running counter.
 *    Depending on compiler settings it will be 32 or 64 bit to match an unsigned long
 *    It overflows every 4295 seconds on 32 bit unsigned long,18446884532025 seconds on 64 bit unsigned longs
 *
 */
unsigned long clkcount(void)
{
	unsigned long count;
#if __SIZEOF_LONG__ == 8											// GCC compiler will set this its specified
	uint32_t hicount;
	uint32_t loCount;
	do {
		hicount = SYSTEMTIMER->CHI; 								// Read Arm system timer high count
		lowCount = SYSTEMTIMER->CLO;								// Read Arm system timer low count
	} while (hicount != SYSTEMTIMER->CHI);							// Check hi counter hasn't rolled during last read
	count = ((uint64_t)hicount << 32) | lowcount;					// Join the 32 bit values to a full 64 bit
#else
	count = SYSTEM_TIMER->CLO;

#endif
	return count;													// Return the 1Mhz count
}

/* clkupdate() interface is documented in clock.h  */
void clkupdate (unsigned long cycles)
{
	/* To set the timer, we are using C3 (System Timer Compare 3), which is one
	 * of the System Timer's output compare registers not being used by the GPU.
	 * When the low bits (CLO) of the timer match C3, the hardware will generate
	 * a timer interrupt.  */

	 /* First clear the current interrupt, if any, by clearing the
	  * BCM2835_SYSTEM_TIMER_MATCH_3 bit in the timer control/status register.
	  * According to the Broadcom documentation, this must be done by _writing_ a
	  * 1 to the bit to be cleared. */
	SYSTEM_TIMER->CS = BCM2835_SYSTEM_TIMER_MATCH_3;

	/* Next, set up the new value in the C3 output compare register.  This is
	 * done by reading the current low-order bits of the counter and adding the
	 * requested number of cycles.  Note that wraparounds will necessarily be
	 * handled correctly because the output compare registers are by design only
	 * 32 bits.  */
	SYSTEM_TIMER->C3 = SYSTEM_TIMER->CLO + cycles;
}
