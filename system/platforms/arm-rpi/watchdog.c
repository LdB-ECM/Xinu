/**
 * @file watchdog.c
 *
 * This file contains code to set the Watchdog Timer on the BCM2835 SoC used on
 * the Raspberry Pi.  When enabled, the Watchdog Timer must be refreshed before
 * it expires, otherwise the system resets itself.
 */
/* Embedded Xinu, Copyright (C) 2013, 2018.  All rights reserved */

#include <stdint.h>
#include <xinu.h>
#include "rpi-platform.h"

/*--------------------------------------------------------------------------}
;{			  RASPBERRY PI WATCHDOG REGISTER LOCATIONS						}
;{-------------------------------------------------------------------------*/
#define PM_RSTC ((volatile __attribute__((aligned(4))) uint32_t*)(RPi_IO_Base_Addr + 0x10001C))
#define PM_WDOG ((volatile __attribute__((aligned(4))) uint32_t*)(RPi_IO_Base_Addr + 0x100024))

#define PM_PASSWORD                   0x5a000000
#define PM_RSTC_WRCFG_CLR             0xffffffcf
#define PM_RSTC_WRCFG_FULL_RESET      0x00000020

#define PM_WDOG_UNITS_PER_SECOND      (1 << 16)
#define PM_WDOG_UNITS_PER_MILLISECOND (PM_WDOG_UNITS_PER_SECOND / 1000)

/**
 * Set the watchdog timer.
 *
 * @param msecs
 *      Milliseconds to wait until the system resets itself.
 *
 * @return
 *      OK
 */
xinu_syscall watchdogset (unsigned int msecs)
{
    *PM_WDOG = PM_PASSWORD | (msecs * PM_WDOG_UNITS_PER_MILLISECOND);
    uint32_t cur = *PM_RSTC;
    *PM_RSTC = PM_PASSWORD | (cur & PM_RSTC_WRCFG_CLR) | PM_RSTC_WRCFG_FULL_RESET;
    return OK;
}
