#include <stdbool.h>
#include <stdint.h>
#include <kernel.h>
#include "CriticalSection.h"

static bool inCrit = true;
static uint32_t intmask = 0;

extern uint32_t disable(void);        // In assembler file system/arch/arm/intutils.S
extern void restore(uint32_t mask);   // In assembler file system/arch/arm/intutils.S

void ENTER_KERNEL_CRITICAL_SECTION (void)
{
	if (inCrit == false) {
		kprintf("Aborting .. ENTER_KERNEL_CRITICAL_SECTION called twice without leaving \n");
		while (1) {}
	}
	intmask = disable();
	inCrit = true;
}

void EXIT_KERNEL_CRITICAL_SECTION (void)
{
	if (inCrit == false) {
		kprintf("Aborting .. EXIT_KERNEL_CRITICAL_SECTION called twice without entering \n");
		while (1) {}
	}
	restore(intmask);
}