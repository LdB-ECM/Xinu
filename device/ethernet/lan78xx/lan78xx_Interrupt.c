
#include <string.h>
#include <xinu.h>
#include <ether.h>

#include <usb_util.h>			// needed for usb_status_t
#include <usb_core_driver.h>	// needed for usb_xfer_request

#include <bufpool.h>
#include "lan78xx.h"

void lan78xx_tx_complete (struct usb_xfer_request *req)
{
    struct ether *ethptr = req->private;

    ethptr->txirq++;
    usb_dev_debug(req->dev, "LAN78xx: Tx complete\n");
    buffree(req);
}

void lan78xx_rx_complete (struct usb_xfer_request *req)
{
    struct ether *ethptr = req->private;

    ethptr->rxirq++;
    if (req->status == USB_STATUS_SUCCESS)
    {
        const uint8_t *data, *edata;
        uint32_t frame_length;

		uint32_t rx_cmd_a; // , rx_cmd_b;
		//uint16_t rx_cmd_c;

        /* For each Ethernet frame in the received USB data... */
        for (data = req->recvbuf, edata = req->recvbuf + req->actual_size;
             data + RX_OVERHEAD + ETH_HDR_LEN + ETH_CRC_LEN <= edata;
             data += RX_OVERHEAD + ((frame_length + 3) & ~3))
        {
            /* Get the RxA, RxB, RxC status word, which contains information about the next
             * Ethernet frame.  */
			rx_cmd_a = data[0] | data[1] << 8 | data[2] << 16 | data[3] << 24;
			//rx_cmd_b = data[4] | data[5] << 8 | data[6] << 16 | data[7] << 24;
			//rx_cmd_c = data[8] | data[9] << 8;


            /* Extract frame_length, which specifies the length of the next
             * Ethernet frame from the MAC destination address to end of the CRC
             * following the payload.  (This does not include the Rx status
             * words, which we instead account for in RX_OVERHEAD.) */
            frame_length = (rx_cmd_a & RX_CMD_A_LEN_MASK_);
			//kprintf("Rx Pkt length: %x Rxa: %08x\n", frame_length, rx_cmd_a);
			
            if ((rx_cmd_a & RX_CMD_A_RED_) ||
                (frame_length + RX_OVERHEAD > edata - data) ||
                (frame_length > ETH_MAX_PKT_LEN + ETH_CRC_LEN) ||
                (frame_length < ETH_HDR_LEN + ETH_CRC_LEN))
            {
                /* The Ethernet adapter set the error flag to indicate a problem
                 * or the Ethernet frame size it provided was invalid. */
                usb_dev_debug(req->dev, "LAN78XX: Tallying rx error "
                              "(recv_status=0x%08x, frame_length=%u)\n",
                              recv_status, frame_length);
                ethptr->errors++;
            }
            else if (ethptr->icount == ETH_IBLEN)
            {
                /* No space to buffer another received packet.  */
                usb_dev_debug(req->dev, "LAN78XX: Tallying overrun\n");
                ethptr->ovrrun++;
            }
            else
            {
                /* Buffer the received packet.  */

                struct ethPktBuffer *pkt;

                pkt = bufget(ethptr->inPool);
                pkt->buf = pkt->data = (uint8_t*)(pkt + 1);
                pkt->length = frame_length - ETH_CRC_LEN;
				memcpy(pkt->buf, data + RX_OVERHEAD, pkt->length);
                ethptr->in[(ethptr->istart + ethptr->icount) % ETH_IBLEN] = pkt;
                ethptr->icount++;

                usb_dev_debug(req->dev, "LAN78XX: Receiving "
                              "packet (length=%u, icount=%u)\n",
                              pkt->length, ethptr->icount);

                /* This may wake up a thread in etherRead().  */
                signal(ethptr->isema);
            }
        }
    }
    else
    {
        /* USB transfer failed for some reason.  */
        usb_dev_debug(req->dev, "LAN78XX: USB Rx transfer failed\n");
        ethptr->errors++;
    }
    usb_dev_debug(req->dev, "LAN78XX: Re-submitting USB Rx request\n");
    usb_submit_xfer_request(req);
}
