/**
 * @file etherInit.c
 *
 * Initialization for the SMSC LAN9512 USB Ethernet Adapter.
 */
/* Embedded Xinu, Copyright (C) 2013, 2018.  All rights reserved. */

#include <xinu.h>

#include <clock.h>
#include <ether.h>
#include <memory.h>
#include <platform.h>
#include <semaphore.h>
#include <stdlib.h>
#include <usb_core_driver.h>

/* Global table of Ethernet devices.  */
struct ether ethertab[NETHER] = { 0 };

/**
 * Semaphores that indicate whether each given Ethernet device has actually been
 * connected to the system yet (here, attached to the USB).  Could be moved into
 * <code>struct ::ether</code> if other drivers needed it...
 */
semaphore ethsem_attached[NETHER] = { 0 };

static void randomEthAddr(unsigned char addr[ETH_ADDR_LEN])
{
	unsigned int i;
	if (platform.serial_low != 0 && platform.serial_high != 0)
	{
		/* Use value generated from platform's serial number.  The problem here
		 * is that we must generate a 48-bit MAC address from a 64-bit serial
		 * number but avoid mapping multiple serial numbers to the same MAC
		 * address.  This is impossible, so we perform an approximation where we
		 * hash the serial number to remove any obvious non-randomness in the
		 * way serial numbers are assigned, then extract the low 48 bits.  */
		unsigned long long serial_nr, hashval;

		serial_nr = (unsigned long long)platform.serial_low |
			((unsigned long long)platform.serial_high << 32);
		hashval = serial_nr * 0x9e37fffffffc0001ULL;
		for (i = 0; i < ETH_ADDR_LEN; i++)
		{
			addr[i] = hashval & 0xff;
			hashval >>= 8;
		}
	}
	else
	{
		/* Generate value using the C library's random number generator, seeded
		 * on the current system timer tick count.  */
		srand(clkcount());
		for (i = 0; i < ETH_ADDR_LEN; i++)
		{
			addr[i] = rand();
		}
	}
	/* Clear multicast bit and set locally assigned bit */
	addr[0] &= 0xfe;
	addr[0] |= 0x02;
}


/* Implementation of etherInit() for the smsc9512; see the documentation for
 * this function in ether.h.  */
/**
 * @details
 * 
 * SMSC LAN9512-specific notes:  This function returns ::OK if the Ethernet
 * driver was successfully registered with the USB core, otherwise ::SYSERR.
 * This is a work-around to use USB's dynamic device model at the same time as
 * Xinu's static device model, and there is no guarantee that the device
 * actually exists when this function returns.  (If it doesn't, the problem is
 * delayed until the device is actually opened with etherOpen()).
 */
xinu_devcall etherInit (device *devptr)
{
    struct ether *ethptr;
    usb_status_t status;

    /* Initialize the static `struct ether' for this device.  */
    ethptr = &ethertab[devptr->minor];
    // bzero(ethptr, sizeof(struct ether)); /* can't do that install sets function ptrs */
    ethptr->dev = devptr;
    ethptr->state = ETH_STATE_DOWN;
    ethptr->mtu = ETH_MTU;
    ethptr->addressLength = ETH_ADDR_LEN;
    ethptr->isema = semcreate(0);
    if (isbadsem(ethptr->isema))
    {
        goto err;
    }

	ethsem_attached[devptr->minor] = semcreate(0);

    if (isbadsem(ethsem_attached[devptr->minor]))
    {
        goto err_free_isema;
    }

    /* The SMSC LAN9512 on the Raspberry Pi does not have an EEPROM attached.
     * The EEPROM is normally used to store the MAC address of the adapter,
     * along with some other information.  As a result, software needs to set
     * the MAC address to a value of its choosing (such as a random number).  */
    randomEthAddr(ethptr->devAddress);

    /* Register this device driver with the USB core and return.  */
    status = usb_register_device_driver(ethptr->specific_driver_header);
    if (status != USB_STATUS_SUCCESS)
    {
        goto err_free_attached_sema;
    }
    return OK;

err_free_attached_sema:
    semfree(ethsem_attached[devptr->minor]);
err_free_isema:
    semfree(ethptr->isema);
err:
    return SYSERR;
}
