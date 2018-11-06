/**
 * @file etherInit.c
 *
 * Initialization for the SMSC LAN9512 USB Ethernet Adapter.
 */
/* Embedded Xinu, Copyright (C) 2013, 2018.  All rights reserved. */

#include <stdbool.h>
#include <stdint.h>
#include <ether.h>
#include <usb_core_driver.h>
#include "lan78xx.h"


/** The initialize sequence comes from the linux driver 
 *  https://github.com/torvalds/linux/blob/master/drivers/net/usb/lan78xx.c
 */




/**
 * Try to bind the LAN78xx driver to a specific USB device.  This the @ref
 * usb_device_driver::bind_device "bind_device" implementation for the 
 * LAN78xx driver and therefore complies with its documented behavior.
 */
static usb_status_t lan78xx_bind_device (struct usb_device *udev)
{
    struct ether *ethptr;

    /* Check if this is actually a LAN78xx by checking the USB device's
     * standard device descriptor, which the USB core already read into memory.
     * Also check to make sure the expected endpoints for sending/receiving
     * packets are present and that the device is operating at high speed.  */
    if (udev->descriptor.idVendor != LAN78XX_VENDOR_ID ||
        udev->descriptor.idProduct != LAN78XX_PRODUCT_ID ||
        udev->interfaces[0]->bNumEndpoints < 2 ||
        (udev->endpoints[0][0]->bmAttributes & 0x3) != USB_TRANSFER_TYPE_BULK ||
        (udev->endpoints[0][1]->bmAttributes & 0x3) != USB_TRANSFER_TYPE_BULK ||
        (udev->endpoints[0][0]->bEndpointAddress >> 7) != USB_DIRECTION_IN ||
        (udev->endpoints[0][1]->bEndpointAddress >> 7) != USB_DIRECTION_OUT ||
        udev->speed != USB_SPEED_HIGH)
    {
        return USB_STATUS_DEVICE_UNSUPPORTED;
    }

	kprintf("Bind: LAN78xx Ethernet product\n");

    /* Make sure this driver isn't already bound to a LAN78xx.
     * TODO: Support multiple devices of this type concurrently.  */
    ethptr = &ethertab[0];
    STATIC_ASSERT(NETHER == 1);
    if (ethptr->csr != NULL)
    {
        return USB_STATUS_DEVICE_UNSUPPORTED;
    }

    ethptr->csr = udev;
    udev->driver_private = ethptr;
    signal(ethsem_attached[ethptr - ethertab]);
    return USB_STATUS_SUCCESS;
}

static void lan78xx_unbind_device(struct usb_device *udev)
{
    struct ether *ethptr = udev->driver_private;

    /* Reset attached semaphore to 0.  */
    wait(ethsem_attached[ethptr - ethertab]);

    /* Close the device.  */
    etherClose(ethptr->dev);
}

usb_status_t lan78xx_wait_device_attached (unsigned short minor)
{
    wait(ethsem_attached[minor]);
    signal(ethsem_attached[minor]);
    return USB_STATUS_SUCCESS;
}

const struct usb_device_driver lan78xx_driver =
{
	.name = "LAN78xx USB Ethernet Adapter Driver",
	.bind_device = lan78xx_bind_device,
	.unbind_device = lan78xx_unbind_device,
};
