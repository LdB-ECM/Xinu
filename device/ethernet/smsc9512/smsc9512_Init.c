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
#include "smsc9512.h"




/**
 * Try to bind the SMSC LAN9512 driver to a specific USB device.  This the @ref
 * usb_device_driver::bind_device "bind_device" implementation for the SMSC
 * LAN9512 driver and therefore complies with its documented behavior.
 */
static usb_status_t smsc9512_bind_device (struct usb_device *udev)
{
    struct ether *ethptr;

    /* Check if this is actually a SMSC LAN9512 by checking the USB device's
     * standard device descriptor, which the USB core already read into memory.
     * Also check to make sure the expected endpoints for sending/receiving
     * packets are present and that the device is operating at high speed.  */
    if (udev->descriptor.idVendor != SMSC9512_VENDOR_ID ||
        udev->descriptor.idProduct != SMSC9512_PRODUCT_ID ||
        udev->interfaces[0]->bNumEndpoints < 2 ||
        (udev->endpoints[0][0]->bmAttributes & 0x3) != USB_TRANSFER_TYPE_BULK ||
        (udev->endpoints[0][1]->bmAttributes & 0x3) != USB_TRANSFER_TYPE_BULK ||
        (udev->endpoints[0][0]->bEndpointAddress >> 7) != USB_DIRECTION_IN ||
        (udev->endpoints[0][1]->bEndpointAddress >> 7) != USB_DIRECTION_OUT ||
        udev->speed != USB_SPEED_HIGH)
    {
        return USB_STATUS_DEVICE_UNSUPPORTED;
    }

	kprintf("Bind: SMC9512 Ethernet product\r\n");

    /* Make sure this driver isn't already bound to a SMSC LAN9512.
     * TODO: Support multiple devices of this type concurrently.  */
    ethptr = &ethertab[0];
    STATIC_ASSERT(NETHER == 1);
    if (ethptr->csr != NULL)
    {
        return USB_STATUS_DEVICE_UNSUPPORTED;
    }

    /* The rest of this function is responsible for making the SMSC LAN9512
     * ready to use, but not actually enabling Rx and Tx (which is done in
     * etherOpen()).  This primarily involves writing to the registers on the
     * SMSC LAN9512.  But these are not memory mapped registers, as this is a
     * USB Ethernet Adapter that is attached on the USB!  Instead, registers are
     * read and written using USB control transfers.  It's somewhat of a pain,
     * and also unlike memory accesses it is possible for USB control transfers
     * to fail.  However, here we perform lazy error checking where we just do
     * all the needed reads and writes, then check at the end if an error
     * occurred.  */

    udev->last_error = USB_STATUS_SUCCESS;

    /* Resetting the SMSC LAN9512 via its registers should not be necessary
     * because the USB code already performed a reset on the USB port it's
     * attached to.  */

    /* Set MAC address.  */
    smsc9512_set_mac_address(udev, ethptr->devAddress);

    /* Allow multiple Ethernet frames to be received in a single USB transfer.
     * Also set a couple flags of unknown function.  */
    smsc9512_set_reg_bits(udev, HW_CFG, HW_CFG_MEF | HW_CFG_BIR | HW_CFG_BCE);

    /* Set the maximum USB (not networking!) packets per USB Rx transfer.
     * Required when HW_CFG_MEF was set.  */
    smsc9512_write_reg(udev, BURST_CAP,
		SMSC9512_DEFAULT_HS_BURST_CAP_SIZE / SMSC9512_HS_USB_PKT_SIZE);

    /* Check for error and return.  */
    if (udev->last_error != USB_STATUS_SUCCESS)
    {
        return udev->last_error;
    }
    ethptr->csr = udev;
    udev->driver_private = ethptr;
    signal(ethsem_attached[ethptr - ethertab]);
    return USB_STATUS_SUCCESS;
}

/**
 * Unbinds the SMSC LAN9512 driver from a SMSC LAN9512 that has been detached.
 * This the @ref usb_device_driver::unbind_device "unbind_device" implementation
 * for the SMSC LAN9512 driver and therefore complies with its documented
 * behavior.
 */
static void smsc9512_unbind_device(struct usb_device *udev)
{
    struct ether *ethptr = udev->driver_private;

    /* Reset attached semaphore to 0.  */
    wait(ethsem_attached[ethptr - ethertab]);

    /* Close the device.  */
    etherClose(ethptr->dev);
}


/**
 * @ingroup etherspecific
 *
 * Wait until the specified Ethernet device has actually been attached.
 *
 * This is necessary because USB is a dynamic bus, but Xinu expects static
 * devices.  Therefore, code opening a static ethernet device must wait until
 * the corresponding USB device has actually been detected and attached.  Fun
 * fact: the USB standard places no constraints on how long this will actually
 * take, even if the device is physically soldered to the board.
 *
 * TODO: Wait for at most a certain amount of time before returning failure.
 *
 * @param minor
 *     Minor number of the Ethernet device to wait for.
 *
 * @return
 *      Currently ::USB_STATUS_SUCCESS.  TODO: implement timeouts and return
 *      ::USB_STATUS_TIMEOUT if timed out.
 */
usb_status_t
smsc9512_wait_device_attached(unsigned short minor)
{
    wait(ethsem_attached[minor]);
    signal(ethsem_attached[minor]);
    return USB_STATUS_SUCCESS;
}


/**
 * Specification of a USB device driver for the SMSC LAN9512.  This is
 * specifically for the USB core and is not related to Xinu's primary device and
 * driver model, which is static.
 */
const struct usb_device_driver smsc9512_driver =
{
	.name = "SMSC LAN9512 USB Ethernet Adapter Driver",
	.bind_device = smsc9512_bind_device,
	.unbind_device = smsc9512_unbind_device,
};
