#ifndef _LAN78XX_H_
#define _LAN78XX_H_

#include <stdint.h>
#include <usb_core_driver.h>


extern const struct usb_device_driver lan78xx_driver;

#define LAN78XX_VENDOR_ID							0x0424
#define LAN78XX_PRODUCT_ID							0x7800

/* THESE ARE PLATFORM STUFF WE NEED FROM CONVERTING THE LINUX DRIVER */
/*           Just linux driver terms to functions we have            */
#define TX_OVERHEAD							(8)
#define RX_OVERHEAD							(10)
#define EIO									(5)				// Error IO = 5
#define EINVAL								(22)			


/* Loop back control */
#define MAC_CR								(0x100)
#define MAC_CR_LOOPBACK_					(0x00000400)

/* USB Vendor Requests */
#define USB_VENDOR_REQUEST_WRITE_REGISTER	(0xA0)
#define USB_VENDOR_REQUEST_READ_REGISTER	(0xA1)
#define USB_VENDOR_REQUEST_GET_STATS		(0xA2)

/* Get and Set MAC address */
#define RX_ADDRL							(0x11C)
#define RX_ADDRL_MASK_						(0xFFFFFFFF)
#define RX_ADDRH							(0x118)
#define RX_ADDRH_MASK_						(0x0000FFFF)

#define MAF_BASE							(0x400)
#define MAF_HIX								(0x00)
#define MAF_LOX								(0x04)
#define NUM_OF_MAF							(33)
#define MAF_HI_BEGIN						(MAF_BASE + MAF_HIX)
#define MAF_LO_BEGIN						(MAF_BASE + MAF_LOX)
#define MAF_HI(index)						(MAF_BASE + (8 * (index)) + (MAF_HIX))
#define MAF_LO(index)						(MAF_BASE + (8 * (index)) + (MAF_LOX))
#define MAF_HI_VALID_						(0x80000000)


/* Read OPT */
#define OTP_BASE_ADDR						(0x00001000)
#define OTP_PWR_DN							(OTP_BASE_ADDR + 4 * 0x00)
#define OTP_PWR_DN_PWRDN_N_					(0x01)
#define OTP_ADDR1							(OTP_BASE_ADDR + 4 * 0x01)
#define OTP_ADDR1_15_11						(0x1F)
#define OTP_ADDR2							(OTP_BASE_ADDR + 4 * 0x02)
#define OTP_ADDR2_10_3						(0xFF)
#define OTP_FUNC_CMD						(OTP_BASE_ADDR + 4 * 0x08)
#define OTP_FUNC_CMD_RESET_					(0x04)
#define OTP_FUNC_CMD_PROGRAM_				(0x02)
#define OTP_FUNC_CMD_READ_					(0x01)
#define OTP_CMD_GO							(OTP_BASE_ADDR + 4 * 0x0A)
#define OTP_CMD_GO_GO_						(0x01)
#define OTP_STATUS							(OTP_BASE_ADDR + 4 * 0x0C)
#define OTP_STATUS_OTP_LOCK_				(0x10)
#define OTP_STATUS_WEB_						(0x08)
#define OTP_STATUS_PGMEN					(0x04)
#define OTP_STATUS_CPUMPEN_					(0x02)
#define OTP_STATUS_BUSY_					(0x01)
#define OTP_RD_DATA							(OTP_BASE_ADDR + 4 * 0x06)

/* Read EEPROM */
#define E2P_CMD								(0x040)
#define E2P_CMD_EPC_BUSY_					(0x80000000)
#define E2P_CMD_EPC_ADDR_MASK_				(0x000001FF)
#define E2P_CMD_EPC_CMD_READ_				(0x00000000)
#define E2P_CMD_EPC_TIMEOUT_				(0x00000400)
#define E2P_DATA							(0x044)

#define HW_CFG								(0x010)
#define HW_CFG_LED1_EN_						(0x00200000)
#define HW_CFG_LED0_EN_						(0x00100000)

#define HW_CFG_LRST_						(0x00000002)
#define USB_CFG0							(0x080)
#define USB_CFG_BIR_						(0x00000040)

#define USB_CFG1							(0x084)
#define USB_CFG1_LTM_ENABLE_				(0x00000100)

#define LTM_BELT_IDLE0						(0x0E0)
#define LTM_BELT_IDLE1						(0x0E4)
#define LTM_BELT_ACT0						(0x0E8)
#define LTM_BELT_ACT1						(0x0EC)
#define LTM_INACTIVE0						(0x0F0)
#define LTM_INACTIVE1						(0x0F4)

#define BURST_CAP							(0x090)
#define BULK_IN_DLY							(0x094)
#define HW_CFG_MEF_							(0x00000010)
#define USB_CFG_BCE_						(0x00000020)
#define FCT_RX_FIFO_END						(0x0C8)
#define FCT_TX_FIFO_END						(0x0CC)
#define INT_STS								(0x0C)
#define INT_STS_CLEAR_ALL_					(0xFFFFFFFF)
#define FLOW								(0x10C)
#define FCT_FLOW							(0x0D0)
#define RFE_CTL								(0x0B0)
#define RFE_CTL_BCAST_EN_					(0x00000400)
#define RFE_CTL_DA_PERFECT_					(0x00000002)

#define PMT_CTL								(0x014)
#define PMT_CTL_PHY_RST_					(0x00000010)
#define PMT_CTL_READY_						(0x00000080)
#define MAC_CR_AUTO_DUPLEX_					(0x00001000)
#define MAC_CR_AUTO_SPEED_					(0x00000800)
#define MAC_TX								(0x108)
#define FCT_TX_CTL							(0x0C4)
#define FCT_TX_CTL_EN_						(0x80000000)
#define MAC_RX								(0x104)
#define MAC_RX_RXEN_						(0x00000001)
#define MAC_RX_MAX_SIZE_MASK_				(0x3FFF0000)
#define MAC_RX_MAX_SIZE_SHIFT_				(16)
#define FCT_RX_CTL							(0x0C0)
#define FCT_RX_CTL_EN_						(0x80000000)
#define MAC_TX_TXEN_						(0x00000001)


#define HW_CFG_LED1_EN_						(0x00200000)
#define HW_CFG_LED0_EN_						(0x00100000)


#define RFE_CTL								(0x0B0)
#define RFE_CTL_IGMP_COE_					(0x00004000)
#define RFE_CTL_ICMP_COE_					(0x00002000)
#define RFE_CTL_TCPUDP_COE_					(0x00001000)
#define RFE_CTL_IP_COE_						(0x00000800)
#define RFE_CTL_BCAST_EN_					(0x00000400)
#define RFE_CTL_MCAST_EN_					(0x00000200)
#define RFE_CTL_UCAST_EN_					(0x00000100)
#define RFE_CTL_VLAN_STRIP_					(0x00000080)
#define RFE_CTL_DISCARD_UNTAGGED_			(0x00000040)
#define RFE_CTL_VLAN_FILTER_				(0x00000020)
#define RFE_CTL_SA_FILTER_					(0x00000010)
#define RFE_CTL_MCAST_HASH_					(0x00000008)
#define RFE_CTL_DA_HASH_					(0x00000004)
#define RFE_CTL_DA_PERFECT_					(0x00000002)
#define RFE_CTL_RST_						(0x00000001)

/* Tx Command A */
#define TX_CMD_A_IGE_						(0x20000000)
#define TX_CMD_A_ICE_						(0x10000000)
#define TX_CMD_A_LSO_						(0x08000000)
#define TX_CMD_A_IPE_						(0x04000000)
#define TX_CMD_A_TPE_						(0x02000000)
#define TX_CMD_A_IVTG_						(0x01000000)
#define TX_CMD_A_RVTG_						(0x00800000)
#define TX_CMD_A_FCS_						(0x00400000)
#define TX_CMD_A_LEN_MASK_					(0x000FFFFF)

/* Tx Command B */
#define TX_CMD_B_MSS_SHIFT_					(16)
#define TX_CMD_B_MSS_MASK_					(0x3FFF0000)
#define TX_CMD_B_MSS_MIN_					((unsigned short)8)
#define TX_CMD_B_VTAG_MASK_					(0x0000FFFF)
#define TX_CMD_B_VTAG_PRI_MASK_				(0x0000E000)
#define TX_CMD_B_VTAG_CFI_MASK_				(0x00001000)
#define TX_CMD_B_VTAG_VID_MASK_				(0x00000FFF)

/* Rx Command A */
#define RX_CMD_A_ICE_						(0x80000000)
#define RX_CMD_A_TCE_						(0x40000000)
#define RX_CMD_A_CSE_MASK_					(0xC0000000)
#define RX_CMD_A_IPV_						(0x20000000)
#define RX_CMD_A_PID_MASK_					(0x18000000)
#define RX_CMD_A_PID_NONE_IP_				(0x00000000)
#define RX_CMD_A_PID_TCP_IP_				(0x08000000)
#define RX_CMD_A_PID_UDP_IP_				(0x10000000)
#define RX_CMD_A_PID_IP_					(0x18000000)
#define RX_CMD_A_PFF_						(0x04000000)
#define RX_CMD_A_BAM_						(0x02000000)
#define RX_CMD_A_MAM_						(0x01000000)
#define RX_CMD_A_FVTG_						(0x00800000)
#define RX_CMD_A_RED_						(0x00400000)
#define RX_CMD_A_RX_ERRS_MASK_				(0xC03F0000)
#define RX_CMD_A_RWT_						(0x00200000)
#define RX_CMD_A_RUNT_						(0x00100000)
#define RX_CMD_A_LONG_						(0x00080000)
#define RX_CMD_A_RXE_						(0x00040000)
#define RX_CMD_A_DRB_						(0x00020000)
#define RX_CMD_A_FCS_						(0x00010000)
#define RX_CMD_A_UAM_						(0x00008000)
#define RX_CMD_A_ICSM_						(0x00004000)
#define RX_CMD_A_LEN_MASK_					(0x00003FFF)

/* Rx Command B */
#define RX_CMD_B_CSUM_SHIFT_				(16)
#define RX_CMD_B_CSUM_MASK_					(0xFFFF0000)
#define RX_CMD_B_VTAG_MASK_					(0x0000FFFF)
#define RX_CMD_B_VTAG_PRI_MASK_				(0x0000E000)
#define RX_CMD_B_VTAG_CFI_MASK_				(0x00001000)
#define RX_CMD_B_VTAG_VID_MASK_				(0x00000FFF)

/* Rx Command C */
#define RX_CMD_C_WAKE_SHIFT_				(15)
#define RX_CMD_C_WAKE_						(0x8000)
#define RX_CMD_C_REF_FAIL_SHIFT_			(14)
#define RX_CMD_C_REF_FAIL_					(0x4000)

/* The 5 basic operations on flags */
usb_status_t lan78xx_write_reg(struct usb_device *udev, uint32_t index, uint32_t data);
usb_status_t lan78xx_read_reg(struct usb_device *udev, uint32_t index, uint32_t *data);

usb_status_t lan78xx_set_mac_address(struct usb_device *udev, const uint8_t *macaddr);
usb_status_t lan78xx_get_mac_address(struct usb_device *udev, uint8_t *macaddr);
usb_status_t lan78xx_wait_device_attached(unsigned short minor);
usb_status_t lan78xx_set_loopback_mode(struct usb_device *udev, unsigned int on_off);

void lan78xx_tx_complete(struct usb_xfer_request *req);
void lan78xx_rx_complete(struct usb_xfer_request *req);

xinu_devcall lan78xxOpen(device *devptr, va_list ap);
xinu_devcall lan78xxWrite(device *devptr, const void *buf, unsigned int len);

#endif                          /* _LAN78XX_H_ */