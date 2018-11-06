/**
 * @file gpioInit.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013, 2018.  All rights reserved. */

#include <xinu.h>
#include <gpio.h>

/**
 * @ingroup gpiogeneric
 *
 * Global table of GPIO devices that are available in the current Embedded Xinu configuration.
 */

struct gpio_device gpiotab[NGPIO] = { 0 };

/**
 * @ingroup gpiogeneric
 *
 * Initialize a GPIO group, including the entry in Xinu's GPIO table as well as the
 * hardware itself.
 *
 * @param devptr
 *      Pointer to the device table entry for the GPIO.
 *
 * @return OK on success; SYSERR on failure.
 */
xinu_devcall gpioInit (device *devptr)
{
    /* Entry for this GPIO in Xinu's GPIO table.  */
    struct gpio_device *gpioptr = &gpiotab[devptr->minor];

	/* Save pointers to the device and the hardware registers in the GPIO structure.  */
	gpioptr->dev = devptr;
	//gpioptr->csr = devptr->csr;

	/* Initialize the semaphore for threads to wait on. */
	gpioptr->iosema = semcreate(0);
	if (isbadsem(gpioptr->iosema))
	{
		return SYSERR;
	}

	/* Initialize the actual hardware.  */
	if (gpioptr->func.pin_initialize)
	{
		semfree(gpioptr->iosema);
		return SYSERR;
	}

    return OK;
}
