#include <stdio.h>
#include <xinu.h>
#include <stdlib.h>
#include <testsuite.h>
#include <network.h>
#include <device.h>
#include <memory.h>
#include <string.h>
#include <ether.h>
#include <interrupt.h>

#if NETHER
static int ethn_test(bool verbose, int dev);

#define ETH_ADDR_LEN 6
#define MAX_PAYLOAD  1516
#define ETH_TYPE_ARP 0x0806

/* Simple ethernet packet structure */
struct etherGram
{
    char dst[ETH_ADDR_LEN];     /* Destination MAC */
    char src[ETH_ADDR_LEN];     /* Source MAC */
    unsigned short type_len;    /* EthernetII type/Ethernet length */
    char payload[1];            /* Payload data */
} __packed;
#endif /* NETHER */

/**
 * Test for ethernet driver packet acceptance.
 */
thread test_ether(bool verbose)
{
#if NETHER
    /* the failif macro depends on 'passed' and 'verbose' vars */
    bool passed = TRUE;
	int i;

	for (i = 0; i < NETHER; i++)
	{
		if (ethertab[i].dev != 0) break;
	}

	if (i == NETHER)
	{
		testSkip(TRUE, "No ethernet installed");
		return OK;
	}

    for (int i = 0; i < NETHER; i++)
    {
        passed &= ethn_test(verbose, i);
    }

    /* always print out the overall tests status */
    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

#else /* NETHER */
    testSkip(TRUE, "");
#endif /* NETHER == 0 */
    return OK;
}

#if NETHER
static int ethn_test(bool verbose, int devminor)
{
	bool passed = TRUE;
	bool subpass;
	unsigned int memsize;
	int i, value, len;
	struct etherGram *inpkt;
	struct etherGram *outpkt;
	char *payload;
	char mymac[ETH_ADDR_LEN];
	char str[80];
	struct ether *peth = &ethertab[devminor];

	int dev = peth->dev->num;

	/* memget */
	memsize = sizeof(struct etherGram) + MAX_PAYLOAD - 1;
	inpkt = memget(memsize);
	outpkt = memget(memsize);

	payload = &(outpkt->payload[0]);

	control(dev, ETH_CTRL_GET_MAC, (long)mymac, 0);
	memcpy(outpkt->dst, mymac, ETH_ADDR_LEN);
	memcpy(outpkt->src, mymac, ETH_ADDR_LEN);
	outpkt->type_len = hs2net(ETH_TYPE_ARP);

	/* generate payload content */
	for (i = 0; i < MAX_PAYLOAD; i++)
	{
		/* Cycle through 0x20 to 0x7d (range of 0x5e) */
		value = (i % 0x5e) + 0x20;
		payload[i] = value;
	}
	payload[0] = 'T';
	payload[1] = 'E';
	payload[2] = 'S';
	payload[3] = 'T';

	/* place ether in loopback mode */
	control(dev, ETH_CTRL_SET_LOOPBK, TRUE, 0);
	sleep(1);

	/* flush any packets already received */
	while (peth->icount > 0)
	{
		read(dev, inpkt, memsize);
	}

	/*uint8_t* p = (uint8_t*)outpkt;
	printf("/nText to loop: %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",
		p[0], p[1],p[2], p[3], p[4], p[5], p[6],p[7],
		p[8], p[9],p[10], p[11], p[12], p[13], p[14], p[15],
		p[16], p[17]);
	write(dev, outpkt, 4+14);
	uint8_t buf[19] = { 0 };
	read(dev, &buf[0], 4+14);
	printf("Looped text: %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\n", 
		buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
		buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15],
		buf[16], buf[17]);*/

	/* flush any packets already received */
	/*while (peth->icount > 0)
	{
		read(dev, inpkt, memsize);
	}*/


    /* oversized packet (paylod 1502 bytes + 14 byte header) */
    sprintf(str, "%s 1516 byte packet", peth->dev->name);
    testPrint(verbose, str);
    len = write(dev, outpkt, 1516);
    failif((SYSERR != len), "");

    /* max packet (payload 1500 bytes + 14 byte header) */
    sprintf(str, "%s 1514 byte packet (write)", peth->dev->name);
    testPrint(verbose, str);
    len = write(dev, outpkt, 1514);
    failif((len < 1514), "");

    sprintf(str, "%s 1514 byte packet (read)", peth->dev->name);
    testPrint(verbose, str);
    bzero(inpkt, memsize);
    len = read(dev, inpkt, 1514);
    failif((0 != memcmp(outpkt, inpkt, 1514)), "");

    /* 'normal' packet (payload 686 bytes + 14 byte header) */
    sprintf(str, "%s  700 byte packet (write)", peth->dev->name);
    testPrint(verbose, str);
    len = write(dev, outpkt, 700);
    failif((len < 700), "");

    sprintf(str, "%s  700 byte packet (read)", peth->dev->name);
    testPrint(verbose, str);
    bzero(inpkt, memsize);
    len = read(dev, inpkt, 700);
    failif((0 != memcmp(outpkt, inpkt, 700)), "");

    /* small packet (payload 16 bytes + 14 byte header) */
    sprintf(str, "%s   30 byte packet (write)", peth->dev->name);
    testPrint(verbose, str);
    len = write(dev, outpkt, 30);
    failif((len < 30), "");

    sprintf(str, "%s   30 byte packet (read)", peth->dev->name);
    testPrint(verbose, str);
    bzero(inpkt, memsize);
    len = read(dev, inpkt, 30);
    failif((0 != memcmp(outpkt, inpkt, 30)), "");

    /* micro packet (12 bytes) */
    sprintf(str, "%s   12 byte packet", peth->dev->name);
    testPrint(verbose, str);
    len = write(dev, outpkt, 12);
    failif((SYSERR != len), "");

    /* send 512 random sized packets */
    sprintf(str, "%s  512 random-sized packets", peth->dev->name);
    testPrint(verbose, str);
    subpass = TRUE;

    for (i = 0; i < 512; i++)
    {
        len = 32 + (rand() % 1200);
        value = write(dev, outpkt, len);
        if (value < len)
        {
            subpass = FALSE;
        }

        bzero(inpkt, memsize);
        value = read(dev, inpkt, len);
        if (0 != memcmp(outpkt, inpkt, 30))
        {
            subpass = FALSE;
        }
    }
    failif((TRUE != subpass), "");

    /* ether out of loopback mode */
    control(dev, ETH_CTRL_SET_LOOPBK, FALSE, 0);

    /* memfree */
    memfree(outpkt, memsize);
    memfree(inpkt, memsize);

    return passed;
}
#endif /* NETHER */
