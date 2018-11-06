
#include <xinu.h>
#include <conf.h>
#include <ether.h>
#include "lan78xx.h"

xinu_devcall lan78xx_Install (unsigned int DevTabNum, const char* devname, unsigned int ethNum)
{
	devtab[DevTabNum].num = DevTabNum;
	devtab[DevTabNum].minor = ethNum;
	devtab[DevTabNum].name = (char*)devname;
	devtab[DevTabNum].init = etherInit;
	devtab[DevTabNum].open = lan78xxOpen;
	devtab[DevTabNum].close = etherClose;
	devtab[DevTabNum].read = etherRead;
	devtab[DevTabNum].write = lan78xxWrite;
	devtab[DevTabNum].seek = 0;
	devtab[DevTabNum].getc = 0;
	devtab[DevTabNum].putc = 0;
	devtab[DevTabNum].control = etherControl;
	devtab[DevTabNum].csr = 0;
	devtab[DevTabNum].intr = 0;
	devtab[DevTabNum].irq = 0;

	ethertab[ethNum].dev = &devtab[DevTabNum];
	/* Set the driver header */
	ethertab[ethNum].specific_driver_header = &lan78xx_driver;

	/* Set the function pointers on install */
	ethertab[ethNum].set_mac_address = lan78xx_set_mac_address;
	ethertab[ethNum].get_mac_address = lan78xx_get_mac_address;
	ethertab[ethNum].set_loopback_mode = lan78xx_set_loopback_mode;
	return DevTabNum;
}