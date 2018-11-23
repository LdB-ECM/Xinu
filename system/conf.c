/* conf.c (GENERATED FILE; DO NOT EDIT) */

#include <conf.h>
#include <device.h>

#include <loopback.h>
#include <null.h>
#include <uart.h>
#include <framebuffer.h>
#include <tty.h>
#include <ether.h>
#include <ethloop.h>
#include <raw.h>
#include <udp.h>
#include <tcp.h>
#include <telnet.h>
#include <usbkbd.h>
#include <kbdmon.h>

/* device independent I/O switch */

device devtab[NDEVS] = { 0 };
