/**
 * @file     lan78xx.c
 *
 * This file provides various functions needed by the LAN78xx USB Ethernet Driver.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <stdbool.h>
#include <stdint.h>
#include <xinu.h>
#include <usb_core_driver.h>
#include "lan78xx.h"


usb_status_t lan78xx_write_reg (struct usb_device *udev, uint32_t index, uint32_t data)
{
	uint32_t temp = data;

    return usb_control_msg(udev, 0,
							USB_VENDOR_REQUEST_WRITE_REGISTER,
							USB_BMREQUESTTYPE_DIR_OUT |
							USB_BMREQUESTTYPE_TYPE_VENDOR |
                            USB_BMREQUESTTYPE_RECIPIENT_DEVICE,
							0, index, &temp, sizeof(uint32_t));
}

usb_status_t lan78xx_read_reg (struct usb_device *udev, uint32_t index, uint32_t *data)
{
    return usb_control_msg(udev, 0,
						    USB_VENDOR_REQUEST_READ_REGISTER,
							USB_BMREQUESTTYPE_DIR_IN |
                            USB_BMREQUESTTYPE_TYPE_VENDOR |
                            USB_BMREQUESTTYPE_RECIPIENT_DEVICE,
			                0, index, data, sizeof(uint32_t));
}


usb_status_t lan78xx_set_mac_address (struct usb_device *udev, const uint8_t *macaddr)
{
    usb_status_t status;
    uint32_t addrl, addrh;

    addrl = macaddr[0] | macaddr[1] << 8 | macaddr[2] << 16 | macaddr[3] << 24;
    addrh = macaddr[4] | macaddr[5] << 8;

    status = lan78xx_write_reg(udev, RX_ADDRL, addrl);
    if (status != USB_STATUS_SUCCESS)
    {
        return status;
    }
    status = lan78xx_write_reg(udev, RX_ADDRH, addrh);
	if (status != USB_STATUS_SUCCESS)
	{
		return status;
	}

	status = lan78xx_write_reg(udev, MAF_LO(0), addrl);
	if (status != USB_STATUS_SUCCESS)
	{
		return status;
	}
	return lan78xx_write_reg(udev, MAF_LO(0), addrh | MAF_HI_VALID_);

}

usb_status_t lan78xx_get_mac_address (struct usb_device *udev, uint8_t *macaddr)
{
    usb_status_t status;
    uint32_t addrl, addrh;

    status = lan78xx_read_reg(udev, RX_ADDRL, &addrl);
    if (status != USB_STATUS_SUCCESS)
    {
        return status;
    }
    status = lan78xx_read_reg(udev, RX_ADDRH, &addrh);
    if (status != USB_STATUS_SUCCESS)
    {
        return status;
    }
    macaddr[0] = (addrl >> 0)  & 0xff;
    macaddr[1] = (addrl >> 8)  & 0xff;
    macaddr[2] = (addrl >> 16) & 0xff;
    macaddr[3] = (addrl >> 24) & 0xff;
    macaddr[4] = (addrh >> 0)  & 0xff;
    macaddr[5] = (addrh >> 8)  & 0xff;
    return USB_STATUS_SUCCESS;
}

usb_status_t lan78xx_set_loopback_mode (struct usb_device *udev, unsigned int on_off)
{
	usb_status_t status;
	uint32_t val;

	status = lan78xx_read_reg(udev, MAC_CR, &val);
	if (status != USB_STATUS_SUCCESS)
	{
		return status;
	}
	val &= ~MAC_CR_LOOPBACK_;
	val |= (on_off == TRUE) ? MAC_CR_LOOPBACK_ : 0;

	status = lan78xx_write_reg(udev, MAC_CR, val);

	return status;
}
