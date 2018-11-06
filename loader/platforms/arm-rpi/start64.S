/**
 * @file start64.S
 *
 * Initialization code for Embedded Xinu on the Raspberry Pi.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

	/* _start:  Entry point of the Xinu kernel.  This will be the very first
	 * byte of the kernel image and on the Raspberry Pi3 will be loaded by
	 * the GPU at address 0x80000.  */
.section ".init", "ax", %progbits
.balign	4
.globl _start
_start:	
//"================================================================"
//  This simply hold startup data for SmartStart its trivial 
//"================================================================"
	adr x12, _start							// Hold boot address in high register R12
	ldr x1, =RPi_BootAddr					// Address of RPi_BootAddr
	str w12, [x1]							// Store the boot address

	ldr	x0, =0x3F000000						// No need to detect address in 64bit mode it has to be 0x3F000000 on Pi3
	ldr x1, =RPi_IO_Base_Addr				// Address of RPi_IO_Base_Addr
	str w0, [x1]							// Store the IO base address

	ldr	x0, =0xC0000000						// No need to detect address in 64bit mode it has to be 0xC0000000 on Pi3
	ldr x1, =RPi_ARM_TO_GPU_Alias			// Address of RPi_IO_Base_Addr
	str w0, [x1]							// Store the IO base address

	mov x0, #1								// Multicore support starts as 1 core
	ldr x1, =RPi_CoresReady					// Address of RPi_CoresReady
	str w0, [x1]							// Store the CoresReady count as 1

	mov x0, #0x98							// Compiled for ARM8 CPU in AARCH64 and supports 4 cores
	ldr x1, =RPi_CompileMode				// Address of RPi_CompileMode
	str w0, [x1]							// Store the compiler mode  

	mrs	x0, midr_el1						// Read CPU Id register
	ldr x1, =RPi_CpuId						// Address of RPi_CpuId
	str w0, [x1]							// Store the CPU id 

//"================================================================"
//  Initialize Generic Timers for Core0
//"================================================================"
	mrs	x0, cnthctl_el2
	orr	x0, x0, #0x3						/* Enable EL1 access to timers */
	msr	cnthctl_el2, x0
	msr	cntvoff_el2, xzr
	
	/* Not sure if I want this yet */
	mrs	x0, cntkctl_el1
	orr	x0, x0, #0x3						/* Enable EL0 access to timers */
	msr	cntkctl_el1, x0

//"================================================================"
//  Setup stack pointers for each core and each CPU operation mode
//"================================================================"
	mrs x6, mpidr_el1						// Read core id on ARM8
	and x6, x6, #0x3						// Make cpu id bitmask
	ldr r1, =(_end-64)						// Address of end address in linker file
	add sp, r1, #NULLSTK					// Add  stack size required
	cbz x6, core_stack_set					// Core0 stack position set
	add sp, r1, #NULLSTK					// Add  stack size required
	cmp x6, #1								// Compare for core1 id
	beq core_stack_set						// Core1 stack position set
	add sp, r1, #NULLSTK					// Add  stack size required
	cmp x6, #2								// Compare for core2 id
	beq core_stack_set						// Core2 stack position set
	add sp, r1, #NULLSTK					// Add  stack size required
core_stack_set:
	mov	x2, sp	
	msr	sp_el2, x2							/* EL2 stack set */
	msr	sp_el1, x2							/* EL1 stack set */
	msr	sp_el0, x2							/* EL0 stack set */

//"================================================================"
//  Initilize MPID/MPIDR registers for all Cores
//"================================================================"
	mrs	x0, midr_el1
	mrs	x1, mpidr_el1
	msr	vpidr_el2, x0
	msr	vmpidr_el2, x1

//"================================================================"
//  Disable coprocessor traps for all Cores
//"================================================================"
	mov	x0, #0x33ff
	msr	cptr_el2, x0						// Disable coprocessor traps to EL2
	msr	hstr_el2, xzr						// Disable coprocessor traps to EL2
	mov	x0, #3 << 20
	msr	cpacr_el1, x0						// Enable FP/SIMD at EL1

//"================================================================"
//  Initialize HCR_EL2 so EL1 is 64 bits for all Cores
//"================================================================"
	mov	x0, #(1 << 31)						// 64bit EL1
	msr	hcr_el2, x0

//"================================================================"
//  Initialize SCTLR_EL1 for all Cores
//"================================================================"
    /*  RES1 bits (29,28,23,22,20,11) to 1
	 *  RES0 bits (31,30,27,21,17,13,10,6) +
	 *  UCI,EE,EOE,WXN,nTWE,nTWI,UCT,DZE,I,UMA,SED,ITD,
	 *  CP15BEN,SA0,SA,C,A,M to 0 */
	mov	x0, #0x0800
	movk	x0, #0x30d0, lsl #16
	orr    x0, x0, #(0x1 << 2)            // The C bit on (data cache). 
	orr    x0, x0, #(0x1 << 12)           // The I bit on (instruction cache)
	msr	sctlr_el1, x0

//"================================================================"
//  Return to the EL1_SP1 mode from EL2 for all Cores
//"================================================================"
	mov	x0, #0x3c5							// EL1_SP1 | D | A | I | F
	msr	spsr_el2, x0						// Set spsr_el2 with settings
	adr	x0, exit_el1						// Address to exit EL2
	msr	elr_el2, x0							// Set elevated return register
	eret									// Call elevated return
exit_el1:

//"================================================================"
//  Set vector table for EL1 for Cores (Yep all use same vectors)
//"================================================================"
    ldr x0, =VectorTable						
    msr vbar_el1,x0

	mrs x6, mpidr_el1						// Read core id on ARM8
	and x6, x6, #0x3						// Make cpu id bitmask
	cbz x6, cpu0_exit_multicore_park		// Core0 continues on

//"================================================================"
//      Now park Core 1,2,3 into secondary spinloop on BCM2837
//"================================================================"
	ldr x1, =RPi_CoresReady					// Address of CoreReady count
	ldr w0, [x1]							// Load current core count 
	add w0, w0, #1							// Add one as core about to go ready
	str w0, [x1]							// Update CoreReady count
	b  StartSecondarySpin					// Jump to setup secondary spin
cpu0_exit_multicore_park:
;@"========================================================================="
@#  Core0 will Clear the .BSS segment other cores will have gone to spinloop
;@"========================================================================="
.core0_clear_bss:
	mov r3, #0x0										;@ Make sure r3 is set to zero (see next test)
	ldr   r0, =__bss_start__							;@ Address of BSS segment start
	ldr   r1, =__bss_end__								;@ Address of BSS segement end
	mov   r2, #0										;@ Zero register R2
.clear_bss:
    cmp   r0, r1										;@ If not at end address
    bge   .clear_bss_exit								;@ Finished clearing section 
    str   r2, [r0]										;@ Store the zeroed register
	add   r0, r0, #4									;@ Add 4 to store address
    b .clear_bss										;@ loop back
.clear_bss_exit:

//"================================================================"
//			Core0 will bring Core 1,2,3 to secondary spin 
//"================================================================"
.equ spin_cpu1, 0xe0
	mov x1, #spin_cpu1						// Spin core1 jump address
	ldr x2, =multicore_start				// Function we are going to call
	str	x2, [x1]							// Store the function address to core1
	sev										// Wake core1 up
 	ldr x3, =RPi_CoresReady					// Set CoresReady count address
.WaitCore1ACK:
	ldr	w1, [x3]							// Read CoresReady count
	cmp	w1, #2								// Wait for setting of second core ready
	bne	.WaitCore1ACK						// Core1 not ready so read again
.equ spin_cpu2, 0xe8
	mov x1, #spin_cpu2						// Spin core2 jump address
	ldr x2, =multicore_start				// Function we are going to call
	str	x2, [x1]							// Store the function address to core2
	sev										// Wake core2 up
 	ldr x3, =RPi_CoresReady					// Set CoresReady count address
.WaitCore2ACK:
	ldr	w1, [x3]							// Read CoresReady count
	cmp	w1, #3								// Wait for setting of third core ready
	bne	.WaitCore2ACK						// Core2 not ready so read again
.equ spin_cpu3, 0xf0
	mov x1, #spin_cpu3						// Spin core3 jump address
	ldr x2, =multicore_start				// Function we are going to call
	str	x2, [x1]							// Store the function address to core3
	sev										// Wake core3 up
 	ldr x3, =RPi_CoresReady					// Set CoresReady count address
.WaitCore3ACK:
	ldr	w1, [x3]							// Read CoresReady count
	cmp	w1, #4								// Wait for setting of third core ready
	bne	.WaitCore3ACK						// Core3 not ready so read again

//"================================================================"
// Finally that all done Core0 jumps to the C compiler entry point
//"================================================================"
	b kernel_main							// Jump out to C kernel 

/*================================================================"
      Just safety incase C code ever returns back
"================================================================*/
hang:
	b hang

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++}
{    Modified bootloader Spin loop but tolerant on registers X0-X7 for C    }
{++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
.equ spin_cpu0, 0xd8
.balign	4
StartSecondarySpin:
	mrs x6, MPIDR_EL1						// Fetch core Id
	and x6, x6, #0x3						// Create 2 bit mask of core Id
	mov x5, #spin_cpu0						// Load address of spins
	mov x1, #0                              // zero x1 register
	str x1, [x5, x6, lsl #3]				// Make sure caller address is zeroed
secondary_spin:
	wfe										// This is all I really want processor to sleep
    ldr x4, [x5, x6, lsl #3]				// Fetch address that has been set
	cbz x4, secondary_spin					// If zero spin 
	mov x0, #0
	str x0, [x5, x6, lsl #3]				// Zero caller address
	mov x1, #0
	mov x2, #0
	mov x3, #0
	blr x4									// Call the function set
	b StartSecondarySpin					// Loop back to spinlock

.balign	4
.ltorg										/* Tell assembler ltorg data for code above can go here */



.balign 0x800
.globl	VectorTable
VectorTable:
	/* from current EL with sp_el0 */
	vector	_start			/* Synchronous */
	vector  hang			/* IRQ */
	vector	hang			/* FIQ */
	vector	hang			/* SErrorStub */

	/* from current EL with sp_elx, x != 0 */
	vector	hang			 /* Synchronous */
	vector	irq_handler_stub /* IRQ */
	vector	hang			 /* FIQ */
	vector	hang			 /* SErrorStub */

	/* from lower EL, target EL minus 1 is AArch64 */
	vector	hang			/* Synchronous */
	vector  hang			/* IRQ */
	vector	hang			/* FIQ */
	vector	hang			/* SErrorStub */

	/* from lower EL, target EL minus 1 is AArch32 */
	vector	hang			/* Synchronous */
	vector  hang			/* IRQ */
	vector	hang			/* FIQ */
	vector	hang			/* SErrorStub */

/****************************************************************
       	   DATA FOR SMARTSTART64  EXPOSED TO INTERFACE 
****************************************************************/
.section ".data.smartstart64", "aw"
.balign 4

.globl RPi_IO_Base_Addr;				// Make sure Pi_IO_Base_Addr label is global
RPi_IO_Base_Addr : .4byte 0;			// Peripheral Base addr is 4 byte variable in 64bit mode

.globl RPi_ARM_TO_GPU_Alias;			// Make sure RPi_ARM_TO_GPU_Alias label is global
RPi_ARM_TO_GPU_Alias: .4byte 0;			// ARM to GPU alias is 4 byte variable in 32bit mode

.globl RPi_BootAddr;					// Make sure RPi_BootAddr label is global
RPi_BootAddr : .4byte 0;				// CPU boot address is 4 byte variable in 64bit mode

.globl RPi_CoresReady;					// Make sure RPi_CoresReady label is global
RPi_CoresReady : .4byte 0;				// CPU cores ready for use is 4 byte variable in 32bit mode

.globl RPi_CPUBootMode;					// Make sure RPi_CPUBootMode label is global
RPi_CPUBootMode : .4byte 0;				// CPU Boot Mode is 4 byte variable in 64bit mode

.globl RPi_CpuId;						// Make sure RPi_CpuId label is global
RPi_CpuId : .4byte 0;					// CPU Id is 4 byte variable in 64bit mode

.globl RPi_CompileMode;					// Make sure RPi_CompileMode label is global
RPi_CompileMode : .4byte 0;				// Compile mode is 4 byte variable in 64bit mode

.globl RPi_SmartStartVer;				// Make sure RPi_SmartStartVer label is global
RPi_SmartStartVer : .4byte 0x00020007;  // SmartStart version is 4 byte variable in 32bit mode
