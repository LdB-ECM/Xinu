SMSC LAN9512
============

This page describes Embedded Xinu's (or :doc:`XinuPi`'s) driver for
the SMSC LAN9512 USB Ethernet Adapter, which is the Ethernet adapter
integrated into the :doc:`Raspberry-Pi` Model B.

Driver overview
---------------

The driver is implemented in :source:`device/smsc9512/`.

Since the SMSC LAN9512 is a USB device, :doc:`/features/USB` support
must be enabled to use it.

Other operating systems name the equivalent driver smsc95xx or similar
because SMSC apparently makes several very similar devices that differ
only in these last two digits.  However as of this writing, this
driver has only been tested with the SMSC LAN9512.

Actually, the SMSC LAN9512 is an integrated USB hub *and* Ethernet
adapter.  The Ethernet adapter itself appears on the USB as a separate
device attached to this hub.  And in fact, the USB ports you can plug
stuff into on the Raspberry Pi model B are other ports attached to
this hub.  Fortunately, this detail is handled by the USB subsystem,
which will automatically use the hub driver for the hub and this
driver for the actual Ethernet adapter device.

Device model
------------

This driver was written for the Raspberry Pi Model B, where the SMSC
LAN9512 is permanently attached to the board.  For this reason and for
compatibility with the rest of Xinu, this driver conforms to Xinu's
static device model.  It therefore cannot be expected to work (yet) to
physically plug an external SMSC LAN9512 into a USB port on the board,
or to enable this driver on a system that does not in fact have an
integrated SMSC LAN9512.  But this could be implemented if needed,
since Xinu's USB subsystem already supports dynamic attachment and
detachment of devices.

MAC address generation
----------------------

To have a predetermined MAC address, a given SMSC LAN9512 must be
attached to an EEPROM that contains the MAC address.  But on the
Raspberry Pi Model B, this EEPROM is not present; therefore, this
driver must assign a MAC address itself.  We do this by generating a
MAC address from the board's serial number.  This guarantees that a
given Raspberry Pi will always have the same MAC address and that two
Raspberry Pis are extremely unlikely to be assigned the same MAC
address.

Sending and receiving packets
-----------------------------

Packets are sent and received by calling ``etherWrite()`` and
``etherRead()`` just like in Xinu's other Ethernet drivers.  The rest
of this section therefore describes driver-internal details.

As seems to be fairly standard for USB Ethernet adapters, networking
packets are sent and received over the USB using USB bulk transfers
to/from the USB Ethernet adapter device.  These transfers contain,
more or less, some device-specific data followed by the raw packet
data.  See the code for details.

The specific details of how USB bulk transfers operate are internal to
the :doc:`USB subsystem </features/USB>`.

Hardware documentation
----------------------

As far as we know, there exists no "Programmers Reference Manual" or
similar for this device.  Therefore, to write this driver we had to
glean the necessary hardware details from the driver that SMSC
contributed to Linux.  In the file
:source:`device/smsc9512/smsc9512.h` we have attempted to document
some of the registers we use in our driver, since the corresponding
definitions in the Linux driver contain no documentation.

Unimplemented features
----------------------

- Support for other potentially compatible SMSC LAN95xx devices
- Dynamically attaching/detaching the device
- Reading from attached EEPROM
- VLANs
- TCP/IP checksum offload
- Suspend
- Wake-on-LAN
- Various PHY features
