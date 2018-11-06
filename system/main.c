/**
 * @file     main.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <device.h>
#include <ether.h>
#include <platform.h>
#include <shell.h>
#include <stdio.h>
#include <thread.h>
#include <version.h>
#include <conf.h>

static void print_os_info(void);

/**
 * Main thread.  You can modify this routine to customize what Embedded Xinu
 * does when it starts up.  The default is designed to do something reasonable
 * on all platforms based on the devices and features configured.
 */
thread main(void)
{
#if HAVE_SHELL
    int shelldevs[4][3];
    unsigned int nshells = 0;
#endif

	kprint_enable = true;
    /* Print information about the operating system  */
    print_os_info();

    /* Open all ethernet devices */
#if NETHER
    {
        unsigned int i;
        for (i = 0; i < NETHER; i++)
        {
			/* Ethernet devive ptr must be must be valid before trying to open */
			/* Otherwise pass harmlessly thru. */
            if ((ethertab[i].dev) && (SYSERR == open(ethertab[i].dev->num)))
            {
                kprintf("WARNING: Failed to open %s\r\n",
                        ethertab[i].dev->name);
            }
        }
    }
#endif /* NETHER */

    /* Set up the first TTY (CONSOLE)  */
#if defined(CONSOLE) && defined(SERIAL0)
    if (OK == open(CONSOLE, SERIAL0))
    {
  #if HAVE_SHELL
        shelldevs[nshells][0] = CONSOLE;
        shelldevs[nshells][1] = CONSOLE;
        shelldevs[nshells][2] = CONSOLE;
        nshells++;
  #endif
    }
    else
    {
        kprintf("WARNING: Can't open CONSOLE over SERIAL0\r\n");
    }
#elif defined(SERIAL0)
  #warning "No TTY for SERIAL0"
#endif

    /* Set up the second TTY (TTY1) if possible  */
#if defined(TTY1)
  #if defined(KBDMON0)
    /* Associate TTY1 with keyboard and use framebuffer output  */
    if (OK == open(TTY1, KBDMON0))
    {
    #if HAVE_SHELL
        shelldevs[nshells][0] = TTY1;
        shelldevs[nshells][1] = TTY1;
        shelldevs[nshells][2] = TTY1;
        nshells++;
    #endif
    }
    else
    {
        kprintf("WARNING: Can't open TTY1 over KBDMON0\r\n");
    }
  #elif defined(SERIAL1)
    /* Associate TTY1 with SERIAL1  */
    if (OK == open(TTY1, SERIAL1))
    {
    #if HAVE_SHELL
        shelldevs[nshells][0] = TTY1;
        shelldevs[nshells][1] = TTY1;
        shelldevs[nshells][2] = TTY1;
        nshells++;
    #endif
    }
    else
    {
        kprintf("WARNING: Can't open TTY1 over SERIAL1\r\n");
    }
  #endif /* SERIAL1 */
#else /* TTY1 */
  #if defined(KBDMON0)
    #warning "No TTY for KBDMON0"
  #elif defined(SERIAL1)
    #warning "No TTY for SERIAL1"
  #endif
#endif /* TTY1 */

    /* Start shells  */
#if HAVE_SHELL
    {
        unsigned int i;
        char name[16];

        for (i = 0; i < nshells; i++)
        {
            sprintf(name, "SHELL%u", i);
            if (SYSERR == ready(create
                                (shell, INITSTK, INITPRIO, name, 3,
                                 shelldevs[i][0],
                                 shelldevs[i][1],
                                 shelldevs[i][2])))
            {
                kprintf("WARNING: Failed to create %s", name);
            }
        }
    }
#endif

    return 0;
}

static void print_os_info(void)
{
	
    kprintf("\r\n");

//#ifdef DETAIL
    /* Output detected platform. */
    //kprintf("Processor identification: 0x%08X\r\n", cpuid);
    kprintf("\nDetected platform as: %s, %s\r\n",
             platform.family, platform.name);
	kprintf("Details: %s\r\n", platform.details);

//#endif

    /* Output Xinu memory layout */
    kprintf("%10d bytes physical memory.\r\n",
            platform.maxaddr - platform.minaddr);
#ifdef DETAIL
    kprintf("           [0x%08X to 0x%08X]\r\n",
            platform.minaddr, (platform.maxaddr - 1));
#endif


    kprintf("%10d bytes reserved system area.\r\n",
            (uintptr_t)&_start - platform.minaddr);
#ifdef DETAIL
    kprintf("           [0x%08X to 0x%08X]\r\n",
            platform.minaddr, (uintptr_t)&_start - 1);
#endif

    kprintf("%10d bytes Xinu code.\r\n", (uintptr_t)&_etext - (uintptr_t)&_start);
#ifdef DETAIL
    kprintf("           [0x%08X to 0x%08X]\r\n",
            (uintptr_t)&_start, (uintptr_t)&_end - 1);
#endif

    kprintf("%10d bytes stack space.\r\n", memheap - (uintptr_t)&_end);
#ifdef DETAIL
    kprintf("           [0x%08X to 0x%08X]\r\n",
            (uintptr_t)&_end, memheap - 1);
#endif

    kprintf("%10d bytes heap space.\r\n",
            platform.maxaddr - memheap);
#ifdef DETAIL
    kprintf("           [0x%08X to 0x%08X]\r\n\r\n",
            memheap, platform.maxaddr - 1);
#endif
    kprintf("\r\n");
}
