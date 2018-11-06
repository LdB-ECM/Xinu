#include <stdint.h>
#include <string.h>
#include <xinu.h>
#include <CriticalSection.h>
#include <ether.h>
#include <ether.h>
#include <bufpool.h>
#include <clock.h>		// Needed for clkcount for small time delays
#include <platform.h>	// Needed for platform clock for small time delays
#include <stdio.h>

#include <usb_core_driver.h>
#include "lan78xx.h"

#define OTP_INDICATOR_1				(0xF3)
#define OTP_INDICATOR_2				(0xF7)
#define EEPROM_INDICATOR			(0xA5)

#define DEFAULT_BURST_CAP_SIZE		(MAX_TX_FIFO_SIZE)
#define DEFAULT_BULK_IN_DELAY		(0x0800)
#define FS_USB_PKT_SIZE				(64)
#define HS_USB_PKT_SIZE				(512)
#define SS_USB_PKT_SIZE				(1024)
#define MAX_RX_FIFO_SIZE			(12 * 1024)
#define MAX_TX_FIFO_SIZE			(12 * 1024)

#define jiffies						(clkcount())			// Our clock timer 

static int lan78xx_read_raw_otp(struct usb_device *dev, uint32_t offset,
	uint32_t length, uint8_t *data)
{
	int i;
	uint32_t buf;
	uint64_t timeout;

	lan78xx_read_reg(dev, OTP_PWR_DN, &buf);

	if (buf & OTP_PWR_DN_PWRDN_N_) {
		/* clear it and wait to be cleared */
		lan78xx_write_reg(dev, OTP_PWR_DN, 0);

		/* Haha I love the linux driver expression .. in a jiffy */
		timeout = jiffies + platform.clkfreq;
		do {
			udelay(100);
			lan78xx_read_reg(dev, OTP_PWR_DN, &buf);
			if (clkcount() > timeout) {
				usb_dev_debug(dev, "timeout on OTP_PWR_DN\n");
				return -EIO;
			}
		} while (buf & OTP_PWR_DN_PWRDN_N_);
	}

	for (i = 0; i < length; i++) {
		lan78xx_write_reg(dev, OTP_ADDR1,
			((offset + i) >> 8) & OTP_ADDR1_15_11);
		lan78xx_write_reg(dev, OTP_ADDR2,
			((offset + i) & OTP_ADDR2_10_3));

		lan78xx_write_reg(dev, OTP_FUNC_CMD, OTP_FUNC_CMD_READ_);
		lan78xx_write_reg(dev, OTP_CMD_GO, OTP_CMD_GO_GO_);

		timeout = jiffies + platform.clkfreq;
		do {
			udelay(1);
			lan78xx_read_reg(dev, OTP_STATUS, &buf);
			if (clkcount() > timeout) {
				usb_dev_debug(dev, "timeout on OTP_STATUS\n");
				return -EIO;
			}
		} while (buf & OTP_STATUS_BUSY_);

		lan78xx_read_reg(dev, OTP_RD_DATA, &buf);

		data[i] = (uint8_t)(buf & 0xFF);
	}

	return 0;
}


static int lan78xx_read_otp (struct usb_device *dev, uint32_t offset,
	uint32_t length, uint8_t *data)
{
	uint8_t sig;
	int ret;

	ret = lan78xx_read_raw_otp(dev, 0, 1, &sig);

	if (ret == 0) {
		if (sig == OTP_INDICATOR_1)
			offset = offset;
		else if (sig == OTP_INDICATOR_2)
			offset += 0x100;
		else
			ret = -EINVAL;
		if (!ret)
			ret = lan78xx_read_raw_otp(dev, offset, length, data);
	}

	return ret;
}

static int lan78xx_eeprom_confirm_not_busy(struct usb_device *dev)
{
	uint64_t timeout = jiffies + platform.clkfreq;
	uint32_t val;
	int ret;

	do {
		ret = lan78xx_read_reg(dev, E2P_CMD, &val);
		if (ret < 0)
			return -EIO;

		if (!(val & E2P_CMD_EPC_BUSY_))
			return 0;

		udelay(40);
	} while (clkcount() < timeout);

	usb_dev_debug(dev, "EEPROM is busy\n");
	return -EIO;
}

static int lan78xx_wait_eeprom(struct usb_device *dev)
{
	uint64_t timeout = clkcount() + platform.clkfreq/10;
	uint32_t val;
	int ret;

	do {
		ret = lan78xx_read_reg(dev, E2P_CMD, &val);
		if (ret < 0)
			return -EIO;

		if (!(val & E2P_CMD_EPC_BUSY_) ||
			(val & E2P_CMD_EPC_TIMEOUT_))
			break;
		udelay(40);
	} while (clkcount() < timeout);

	if (val & (E2P_CMD_EPC_TIMEOUT_ | E2P_CMD_EPC_BUSY_)) {
		usb_dev_debug(dev, "EEPROM read operation timeout\n");
		return -EIO;
	}

	return 0;
}

static int lan78xx_read_raw_eeprom(struct usb_device *dev, uint32_t offset,
	uint32_t length, uint8_t *data)
{
	uint32_t val;
	uint32_t saved;
	int i, ret;
	int retval;

	/* depends on chip, some EEPROM pins are muxed with LED function.
	 * disable & restore LED function to access EEPROM.
	 */
	ret = lan78xx_read_reg(dev, HW_CFG, &val);
	saved = val;

	// No need for test it is 78xx on Pi3B+
	val &= ~(HW_CFG_LED1_EN_ | HW_CFG_LED0_EN_);
	ret = lan78xx_write_reg(dev, HW_CFG, val);

	retval = lan78xx_eeprom_confirm_not_busy(dev);
	if (retval)
		return retval;

	for (i = 0; i < length; i++) {
		val = E2P_CMD_EPC_BUSY_ | E2P_CMD_EPC_CMD_READ_;
		val |= (offset & E2P_CMD_EPC_ADDR_MASK_);
		ret = lan78xx_write_reg(dev, E2P_CMD, val);
		if (ret < 0) {
			retval = -EIO;
			goto exit;
		}

		retval = lan78xx_wait_eeprom(dev);
		if (retval < 0)
			goto exit;

		ret = lan78xx_read_reg(dev, E2P_DATA, &val);
		if (ret < 0) {
			retval = -EIO;
			goto exit;
		}

		data[i] = val & 0xFF;
		offset++;
	}

	retval = 0;
exit:
	// Again no need for test it is 78xx on Pi3B+
	ret = lan78xx_write_reg(dev, HW_CFG, saved);

	return retval;
}

static int lan78xx_read_eeprom(struct usb_device *dev, uint32_t offset,
	uint32_t length, uint8_t *data)
{
	uint8_t sig;
	int ret;

	ret = lan78xx_read_raw_eeprom(dev, 0, 1, &sig);
	if ((ret == 0) && (sig == EEPROM_INDICATOR))
		ret = lan78xx_read_raw_eeprom(dev, offset, length, data);
	else
		ret = -EINVAL;

	return ret;
}

static void lan78xx_init_ltm(struct usb_device *dev)
{
	int ret;
	uint32_t buf;
	uint32_t regs[6] = { 0 };

	ret = lan78xx_read_reg(dev, USB_CFG1, &buf);
	if (buf & USB_CFG1_LTM_ENABLE_) {
		uint8_t temp[2];
		/* Get values from EEPROM first */
		if (lan78xx_read_eeprom(dev, 0x3F, 2, temp) == 0) {
			if (temp[0] == 24) {
				ret = lan78xx_read_raw_eeprom(dev,
					temp[1] * 2,
					24,
					(uint8_t *)regs);
				if (ret < 0)
					return;
			}
		}
		else if (lan78xx_read_otp(dev, 0x3F, 2, temp) == 0) {
			if (temp[0] == 24) {
				ret = lan78xx_read_raw_otp(dev,
					temp[1] * 2,
					24,
					(uint8_t *)regs);
				if (ret < 0)
					return;
			}
		}
	}

	lan78xx_write_reg(dev, LTM_BELT_IDLE0, regs[0]);
	lan78xx_write_reg(dev, LTM_BELT_IDLE1, regs[1]);
	lan78xx_write_reg(dev, LTM_BELT_ACT0, regs[2]);
	lan78xx_write_reg(dev, LTM_BELT_ACT1, regs[3]);
	lan78xx_write_reg(dev, LTM_INACTIVE0, regs[4]);
	lan78xx_write_reg(dev, LTM_INACTIVE1, regs[5]);
}

static int lan78xx_set_rx_max_frame_length(struct usb_device *dev, int size)
{
	uint32_t buf;
	bool rxenabled;

	lan78xx_read_reg(dev, MAC_RX, &buf);

	rxenabled = ((buf & MAC_RX_RXEN_) != 0);

	if (rxenabled) {
		buf &= ~MAC_RX_RXEN_;
		lan78xx_write_reg(dev, MAC_RX, buf);
	}

	/* add 4 to size for FCS */
	buf &= ~MAC_RX_MAX_SIZE_MASK_;
	buf |= (((size + 4) << MAC_RX_MAX_SIZE_SHIFT_) & MAC_RX_MAX_SIZE_MASK_);

	lan78xx_write_reg(dev, MAC_RX, buf);

	if (rxenabled) {
		buf |= MAC_RX_RXEN_;
		lan78xx_write_reg(dev, MAC_RX, buf);
	}

	return 0;
}


#define NETIF_F_RXCSUM				4
#define NETIF_F_HW_VLAN_CTAG_RX		2
#define NETIF_F_HW_VLAN_CTAG_FILTER 1

/* Enable or disable Rx checksum offload engine */
static int lan78xx_set_features(struct usb_device *dev, uint32_t features)
{
	uint32_t rfe_ctl;
	lan78xx_read_reg(dev, RFE_CTL, &rfe_ctl);

	if (features & NETIF_F_RXCSUM) {
		rfe_ctl |= RFE_CTL_TCPUDP_COE_ | RFE_CTL_IP_COE_;
		rfe_ctl |= RFE_CTL_ICMP_COE_ | RFE_CTL_IGMP_COE_;
	}
	else {
		rfe_ctl &= ~(RFE_CTL_TCPUDP_COE_ | RFE_CTL_IP_COE_);
		rfe_ctl &= ~(RFE_CTL_ICMP_COE_ | RFE_CTL_IGMP_COE_);
	}

	if (features & NETIF_F_HW_VLAN_CTAG_RX)
		rfe_ctl |= RFE_CTL_VLAN_STRIP_;
	else
		rfe_ctl &= ~RFE_CTL_VLAN_STRIP_;

	if (features & NETIF_F_HW_VLAN_CTAG_FILTER)
		rfe_ctl |= RFE_CTL_VLAN_FILTER_;
	else
		rfe_ctl &= ~RFE_CTL_VLAN_FILTER_;


	lan78xx_write_reg(dev, RFE_CTL, rfe_ctl);

	return 0;
}

static int lan78xx_reset(struct usb_device *dev, uint8_t* macaddress)
{
	int ret = 0;
	uint32_t buf;

	lan78xx_read_reg(dev, HW_CFG, &buf);
	buf |= HW_CFG_LRST_;
	lan78xx_write_reg(dev, HW_CFG, buf);

	uint64_t timeout = jiffies + platform.clkfreq;
	do {
		udelay(1);
		ret = lan78xx_read_reg(dev, HW_CFG, &buf);
		if (clkcount() > timeout) 
		{
			usb_dev_debug(dev, "timeout on completion of LiteReset\n");
			return -EIO;
		}
	} while (buf & HW_CFG_LRST_);


	lan78xx_set_mac_address(dev, macaddress);

	/* Respond to the IN token with a NAK */
	ret = lan78xx_read_reg(dev, USB_CFG0, &buf);
	buf |= USB_CFG_BIR_;
	ret = lan78xx_write_reg(dev, USB_CFG0, buf);

	/* Init LTM */
	lan78xx_init_ltm(dev);

	//buf = DEFAULT_BURST_CAP_SIZE / HS_USB_PKT_SIZE;
	//buf = DEFAULT_BURST_CAP_SIZE / SS_USB_PKT_SIZE;
	buf = DEFAULT_BURST_CAP_SIZE / FS_USB_PKT_SIZE;
	ret = lan78xx_write_reg(dev, BURST_CAP, buf);
	ret = lan78xx_write_reg(dev, BULK_IN_DLY, DEFAULT_BULK_IN_DELAY);

	ret = lan78xx_read_reg(dev, HW_CFG, &buf);
	buf |= HW_CFG_MEF_;
	buf |= HW_CFG_LED0_EN_;
	buf |= HW_CFG_LED1_EN_;
	ret = lan78xx_write_reg(dev, HW_CFG, buf);

	ret = lan78xx_read_reg(dev, USB_CFG0, &buf);
	buf |= USB_CFG_BCE_;
	ret = lan78xx_write_reg(dev, USB_CFG0, buf);

	/* set FIFO sizes */
	buf = (MAX_RX_FIFO_SIZE - 512) / 512;
	ret = lan78xx_write_reg(dev, FCT_RX_FIFO_END, buf);

	buf = (MAX_TX_FIFO_SIZE - 512) / 512;
	ret = lan78xx_write_reg(dev, FCT_TX_FIFO_END, buf);

	ret = lan78xx_write_reg(dev, INT_STS, INT_STS_CLEAR_ALL_);
	ret = lan78xx_write_reg(dev, FLOW, 0);
	ret = lan78xx_write_reg(dev, FCT_FLOW, 0);

	/* Don't need rfe_ctl_lock during initialisation */
	ret = lan78xx_read_reg(dev, RFE_CTL, &buf);
	buf |= (RFE_CTL_BCAST_EN_ | RFE_CTL_DA_PERFECT_);
	ret = lan78xx_write_reg(dev, RFE_CTL, buf);

	/* Enable or disable checksum offload engines */
	lan78xx_set_features(dev, 0);

	//lan78xx_set_multicast(dev->net);
	lan78xx_read_reg(dev, RFE_CTL, &buf);
	buf &= ~(RFE_CTL_UCAST_EN_ | RFE_CTL_MCAST_EN_ |
		RFE_CTL_DA_PERFECT_ | RFE_CTL_MCAST_HASH_);
	ret = lan78xx_write_reg(dev, RFE_CTL, buf);

	/* reset PHY */
	ret = lan78xx_read_reg(dev, PMT_CTL, &buf);
	buf |= PMT_CTL_PHY_RST_;
	ret = lan78xx_write_reg(dev, PMT_CTL, buf);

	timeout = jiffies + platform.clkfreq;
	do {
		udelay(100);
		ret = lan78xx_read_reg(dev, PMT_CTL, &buf);
		if (clkcount() > timeout) {
			usb_dev_debug(dev, "timeout waiting for PHY Reset\n");
			return -EIO;
		}
	} while ((buf & PMT_CTL_PHY_RST_) || !(buf & PMT_CTL_READY_));

	ret = lan78xx_read_reg(dev, MAC_CR, &buf);


	uint8_t sig;
	ret = lan78xx_read_raw_eeprom(dev, 0, 1, &sig);
	if (!ret && sig != EEPROM_INDICATOR) {
		usb_dev_debug(dev, "No External EEPROM. Setting MAC Speed\n");
		buf |= MAC_CR_AUTO_DUPLEX_ | MAC_CR_AUTO_SPEED_;
	}

	ret = lan78xx_write_reg(dev, MAC_CR, buf);

	ret = lan78xx_read_reg(dev, MAC_TX, &buf);
	buf |= MAC_TX_TXEN_;
	ret = lan78xx_write_reg(dev, MAC_TX, buf);

	ret = lan78xx_read_reg(dev, FCT_TX_CTL, &buf);
	buf |= FCT_TX_CTL_EN_;
	ret = lan78xx_write_reg(dev, FCT_TX_CTL, buf);

	ret = lan78xx_set_rx_max_frame_length(dev, ETH_MTU + ETH_VLAN_LEN);

	ret = lan78xx_read_reg(dev, MAC_RX, &buf);
	buf |= MAC_RX_RXEN_;
	ret = lan78xx_write_reg(dev, MAC_RX, buf);

	ret = lan78xx_read_reg(dev, FCT_RX_CTL, &buf);
	buf |= FCT_RX_CTL_EN_;
	ret = lan78xx_write_reg(dev, FCT_RX_CTL, buf);

	return 0;
}




xinu_devcall lan78xxOpen (device *devptr, va_list ap)
{
    struct ether *ethptr;
    struct usb_device *udev;
    int retval = SYSERR;

	ENTER_KERNEL_CRITICAL_SECTION();

    /* Wait for USB device to actually be attached.  */
    if (lan78xx_wait_device_attached(devptr->minor) != USB_STATUS_SUCCESS)
    {
        goto out_restore;
    }

    /* Fail if device is not down.  */
    ethptr = &ethertab[devptr->minor];
    if (ethptr->state != ETH_STATE_DOWN)
    {
        goto out_restore;
    }

	/* Create buffer pool for Tx transfers.  */
#define  LAN78XX_MAX_TX_REQUESTS 1
	ethptr->outPool = bfpalloc(sizeof(struct usb_xfer_request) + ETH_MAX_PKT_LEN +
		TX_OVERHEAD,
		LAN78XX_MAX_TX_REQUESTS);
	if (ethptr->outPool == SYSERR)
	{
		goto out_restore;
	}

	/* Create buffer pool for Rx packets (not the actual USB transfers, which
	 * are allocated separately).  */
	ethptr->inPool = bfpalloc(sizeof(struct ethPktBuffer) + ETH_MAX_PKT_LEN,
		ETH_IBLEN);
	if (ethptr->inPool == SYSERR)
	{
		goto out_free_out_pool;
	}

    /* We're abusing the csr field to store a pointer to the USB device
     * structure.  At least it's somewhat equivalent, since it's what we need to
     * actually communicate with the device hardware.  */
    udev = ethptr->csr;

	   
	/* The rest of this function is responsible for making the LAN78xx
	  * ready to use, but not actually enabling Rx and Tx (which is done in
	  * etherOpen()).  This primarily involves writing to the registers on the
	  * LAN78xx.  But these are not memory mapped registers, as this is a
	  * USB Ethernet Adapter that is attached on the USB!  Instead, registers are
	  * read and written using USB control transfers.  It's somewhat of a pain,
	  * and also unlike memory accesses it is possible for USB control transfers
	  * to fail.  However, here we perform lazy error checking where we just do
	  * all the needed reads and writes, then check at the end if an error
	  * occurred.  */


	udev->last_error = USB_STATUS_SUCCESS;
	 

	//retval = lan78xx_reset(udev, &ethptr->devAddress[0]);
	/*ethptr->devAddress[0] = 0xB8;
	ethptr->devAddress[1] = 0x27;
	ethptr->devAddress[2] = 0xEB;
	ethptr->devAddress[3] = 0x71;
	ethptr->devAddress[4] = 0x5A;
	ethptr->devAddress[5] = 0x97;*/
	retval = lan78xx_reset(udev, &ethptr->devAddress[0]);
	if (retval < 0) goto out_free_in_pool;

	/* Initialize the Tx requests.  */
	{
		struct usb_xfer_request *reqs[LAN78XX_MAX_TX_REQUESTS];
		for (int i = 0; i < LAN78XX_MAX_TX_REQUESTS; i++)
		{
			struct usb_xfer_request *req;

			req = bufget(ethptr->outPool);
			usb_init_xfer_request(req);
			req->dev = udev;
			/* Assign Tx endpoint, checked in lan78xx_bind_device() */
			req->endpoint_desc = udev->endpoints[0][1];
			req->sendbuf = (uint8_t*)req + sizeof(struct usb_xfer_request);
			req->completion_cb_func = lan78xx_tx_complete;
			req->private = ethptr;
			reqs[i] = req;
		}
		for (int i = 0; i < LAN78XX_MAX_TX_REQUESTS; i++)
		{
			buffree(reqs[i]);
		}
	}

	/* Allocate and submit the Rx requests.  TODO: these aren't freed anywhere.
	 * */
#define LAN78XX_MAX_RX_REQUESTS 1
	for (int i = 0; i < LAN78XX_MAX_RX_REQUESTS; i++)
	{
		struct usb_xfer_request *req;

		req = usb_alloc_xfer_request(DEFAULT_BURST_CAP_SIZE);
		if (req == NULL)
		{
			goto out_free_in_pool;
		}
		req->dev = udev;
		/* Assign Rx endpoint, checked in lan78xx_bind_device() */
		req->endpoint_desc = udev->endpoints[0][0];
		req->completion_cb_func = lan78xx_rx_complete;
		req->private = ethptr;
		usb_submit_xfer_request(req);
	}

    /* Success!  Set the device to ETH_STATE_UP. */
    ethptr->state = ETH_STATE_UP;
    retval = OK;
    goto out_restore;

out_free_in_pool:
    bfpfree(ethptr->inPool);
out_free_out_pool:
    bfpfree(ethptr->outPool);
out_restore:
	EXIT_KERNEL_CRITICAL_SECTION();
    return retval;
}
