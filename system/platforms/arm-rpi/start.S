/**
 * @file start.S
 *
 * Initialization code for Embedded Xinu on the Raspberry Pi.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */
;@"========================================================================="
;@#				 	 ARM CPU ID CONSTANT DEFINITIONS
;@"========================================================================="
.equ ARM6_CPU_ID, 0x410FB767;							;@ CPU id a BCM2835 reports
.equ ARM7_CPU_ID, 0x410FC073;							;@ CPU id a BCM2836 reports
.equ ARM8_CPU_ID, 0x410FD034;							;@ CPU id a BCM2837 reports

;@"========================================================================="
@#			    ARM CPU MODE CONSTANT DEFINITIONS IN CPSR REGISTER
;@"========================================================================="
.equ ARM_MODE_USR, 0x10;								;@ CPU in USR mode .. Normal User mode
.equ ARM_MODE_FIQ, 0x11;								;@ CPU in FIQ mode .. FIQ Processing
.equ ARM_MODE_IRQ, 0x12;								;@ CPU in IRQ mode .. IRQ Processing
.equ ARM_MODE_SVC, 0x13;								;@ CPU in SVC mode .. Service mode
.equ ARM_MODE_HYP, 0x1A;								;@ CPU in HYP mode .. Hypervisor mode  (ARM7/ARM8 only)
.equ ARM_MODE_UND, 0x1B;								;@ CPU in UND mode .. Undefined Instructions mode
.equ ARM_MODE_SYS, 0x1F;								;@ CPU in SYS mode .. Priviledged Operating System Tasks mode

;@"========================================================================="
@#			          MASK CONSTANTS FOR CPSR REGISTER
;@"========================================================================="
.equ ARM_MODE_MASK, 0x1F;								;@ Mask to clear all but CPU mode bits from cpsr register
.equ ARM_I_BIT,		0x80;								;@ IRQs disabled when set to 1
.equ ARM_F_BIT,		0x40;								;@ FIQs disabled when set to 1

#define NULLSTK 8192

.section ".init", "ax", %progbits
.globl _start
.balign 4

	/* _start:  Entry point of the Xinu kernel.  This will be the very first
	 * byte of the kernel image and on the Raspberry Pi will be loaded by
	 * the GPU at address 0x8000.  */
_start:
;@"========================================================================="
@#        Grab cpu mode and start address and hold in a high register.
;@"========================================================================="
	mov r12, pc											;@ Hold boot address in high register R12
	mrs r0, CPSR										;@ Fetch the cpsr register
	orr r0, r0, #(ARM_I_BIT | ARM_F_BIT)				;@ Disable Irq/Fiq
	and r11, r0, #ARM_MODE_MASK							;@ Clear all but CPU mode bits in register r11
;@"========================================================================="
@#      If the cpu is in HYP_MODE(EL2) we will bring it SVC_MODE (EL1).
;@"========================================================================="                        
 	cmp r11, #ARM_MODE_HYP								;@ Check we are in HYP_MODE											
	bne .NotInHypMode									;@ Branch if not equal meaning was not in HYP_MODE  
	bic r0, r0, #ARM_MODE_MASK							;@ Clear the CPU mode bits in register r0							
	orr r0, r0, #ARM_MODE_SVC							;@ ARM_MODE_SVC bits onto register	
    msr spsr_cxsf,r0									;@ Hold value in spsr_cxsf
    add lr,pc,#4										;@ Calculate address of .NotInHypMode label

	/*# I borrowed this trick from Ultibo because ARM6 code running on an ARM7/8 needs this opcode. */
	/*# The ARM6 compiler does not know these instructions as it is not on an ARM6 cpu but we need  */
    /*# the opcode so our ARM6 code can drop an ARM7/ARM8 cpu out of HYP mode. An ARM6 cpu will go  */
	/*# around the code becuase of the initial test for HYP mode. This allow us to write Pi ARM6    */
	/*# code that can run on a Pi with ARM7/ARM8 CPU. Native ARM7/8 compilers already understand    */
	/*# the OPCODE but do not mind that it is entered thise way they just execute the opcode. 	    */        
	.long 0xE12EF30E									;@ "msr ELR_hyp, lr" Set the address to ELR_hyp
	.long 0xE160006E									;@ "eret" Elevated return which will exit at .NotInHypMode in SVC_MODE
.NotInHypMode:

;@"========================================================================="
@#				   Set CPU to SYS_MODE operation
;@"========================================================================="  
	bic r0, r0, #ARM_MODE_MASK							;@ Clear the CPU mode bits in register r0							
	orr r0, r0, #ARM_MODE_SYS							;@ SYS_MODE bits onto register
    msr CPSR_c, r0										;@ Switch back to SYS_MODE which will be our operation mode

	mov	r1, #0
	mcr p15, 0, r1, c12, c0, 0	/* Write to VBAR */

;@"========================================================================="
@#     Check if BC2835 and if so branch over and clear the bss
;@"========================================================================="
	mrc p15,0,r0,c0,c0,0								;@ Read CPU ID Register
	ldr r1, =ARM6_CPU_ID								;@ fetch ARM6_CPU_ID
	cmp r1, r0											;@ Check for match
	beq .core0_clear_bss								;@ Branch as Arm6 has only 1 CPU 
;@"========================================================================="
@#    Core 1,2,3 on BCM2836, BCM2837 jump over BSS setting
;@"========================================================================="
	mrc p15, 0, r3, c0, c0, 5							;@ Read core id on ARM7 & ARM8
	ands r3, r3, #0x3									;@ Make core 2 bit bitmask in R0
	bne  .clear_bss_exit								;@ Core 1,2,3 jumps out
;@"========================================================================="
@#     Core0 will Clear the .BSS segment other cores will jump this
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
;@"========================================================================="
@#						 Set Stack Pointer
;@"========================================================================="  
	ldr r1, =(_end-64)									;@ Address of end address in linker file
	add sp, r1, #NULLSTK								;@ Add  stack size required

	/* The remaining memory available to the ARM will be Xinu "memheap"
	 * region, which is used for dynamic memory allocation.  Set its
	 * starting address.  Only core 0 sets this */
	cmp r3, #0
	bne .Only_Core0_Sets_memheap
	ldr r0, =memheap
	str sp, [r0]
.Only_Core0_Sets_memheap:

.if (__ARM_FP == 12)
;@"========================================================================="
@#    PI NSACR regsister setup for access to floating point unit
@#    Cortex A-7 => Section 4.3.34. Non-Secure Access Control Register
@#    Cortex A-53 => Section 4.5.32. Non-Secure Access Control Register
;@"========================================================================="
	mrc p15, 0, r0, c1, c1, 2							;@ Read NSACR into R0
	cmp r0, #0x00000C00									;@ Access turned on or in AARCH32 mode and can not touch register or EL3 fault
	beq .free_to_enable_fpu1
	orr r0, r0, #0x3<<10								;@ Set access to both secure and non secure modes
	mcr p15, 0, r0, c1, c1, 2							;@ Write NSACR
;@"========================================================================="
@#                               Bring fpu online
;@"========================================================================="
.free_to_enable_fpu1:
	mrc p15, 0, r0, c1, c0, #2							;@ R0 = Access Control Register
	orr r0, #(0x300000 + 0xC00000)						;@ Enable Single & Double Precision
	mcr p15,0,r0,c1,c0, #2								;@ Access Control Register = R0
	mov r0, #0x40000000									;@ R0 = Enable VFP
	vmsr fpexc, r0										;@ FPEXC = R0
.endif

;@"========================================================================="
@#                              Enable L1 cache
;@"========================================================================="
.equ SCTLR_ENABLE_DATA_CACHE,			0x4
.equ SCTLR_ENABLE_BRANCH_PREDICTION,	0x800
.equ SCTLR_ENABLE_INSTRUCTION_CACHE,	0x1000
    mrc p15,0,r0,c1,c0,0								;@ R0 = System Control Register

    /* Enable caches and branch prediction */
    orr r0, #SCTLR_ENABLE_BRANCH_PREDICTION				;@ Branch prediction on
    orr r0, #SCTLR_ENABLE_DATA_CACHE					;@ Data cache on
    orr r0, #SCTLR_ENABLE_INSTRUCTION_CACHE				;@ Instruction cache on

	orr r0, #1 << 22									;@ unaligned access off

    mcr p15,0,r0,c1,c0,0								;@ System Control Register = R0
;@"========================================================================="
@#     Check if BC2835 and if so branch over multi-core park routine
;@"========================================================================="
	mrc p15,0,r0,c0,c0,0								;@ Read CPU ID Register
	ldr r1, =ARM6_CPU_ID								;@ fetch ARM6_CPU_ID
	cmp r1, r0											;@ Check for match
	beq .cpu0_exit_multicore_park						;@ Branch as Arm6 has only 1 CPU 
;@"========================================================================="
@#   Now park Core 1,2,3 into secondary spinloop on BCM2836, BCM2837
@#   If bootloader is used this will not happen but booting with the
@#   "OldKernel=1" from 0x0000 this can happen. Core 0 continues on.
;@"========================================================================="
	mrc p15, 0, r0, c0, c0, 5							;@ Read core id on ARM7 & ARM8
	ands r0, r0, #0x3									;@ Make core 2 bit bitmask in R0
	beq  .cpu0_exit_multicore_park						;@ Core 0 jumps out
	ldr r1, =RPi_CoresReady								;@ Address of CoreReady count
	ldr r0, [r1]										;@ Load current core count 
	add r0, r0, #1										;@ Add one as core about to go ready
	str r0, [r1]										;@ Update CoreReady count
	b    SecondarySpin									;@ Jump to setup secondary spin
.cpu0_exit_multicore_park:
;@"========================================================================="
@#      Now store initial CPU boot mode and address we might need later.
;@"========================================================================="
	ldr r1, =RPi_BootAddr								;@ Address to hold Boot address
	sub r12, #8											;@ Subtract op-code offset
	str r12, [r1]										;@ Save the boot address (r12) we started at
	ldr r1, =RPi_CPUBootMode							;@ Memory address to save this CPU boot mode
	str r11, [r1]										;@ Save the boot mode (r11) we started in
;@"========================================================================="
@#           Initial CPU core ready so set CoreReady count to 1.
;@"========================================================================="
	mov r0, #1											;@ Core0 is will be ready
	ldr r1, =RPi_CoresReady								;@ Memory address to save Cores ready
	str r0, [r1]										;@ Set RPi_CoresReady to 1
;@"========================================================================="
@#     Fetch and hold CPU changed mode. If we changed modes this value
@#     will now reflect a change from the original held RPi_CPUBootMode.
;@"========================================================================="
	mrs r2, CPSR
	and r2, r2, #0x1F									;@ Clear all but CPU mode bits in register r2
	ldr r1, =RPi_CPUCurrentMode
	str r2, [r1]										;@ Hold the changed CPU mode
;@"========================================================================="
@#          Read the Arm Main CPUID register => sets RPi_CpuId
;@"========================================================================="
	ldr r1, =RPi_CpuId									;@ Address to hold CPU id
	mrc p15, 0, r0, c0, c0, 0							;@ Read Main ID Register
	str r0, [r1]										;@ Save CPU Id for interface 
;@"========================================================================="
@#             Store the compiler mode in RPi_CompileMode
;@"========================================================================="
	eor r0, r0, r0;										;@ Zero register
.if (__ARM_ARCH == 6)									// Compiling for ARM6
	mov r0, #0x06										;@ Compiled for ARM6 CPU
.endif
.if (__ARM_ARCH == 7)									// Compiling for ARM7
	mov r0, #0x07										;@ Compiled for ARM7 CPU
.endif
.if (__ARM_ARCH == 8)									// Compiling for ARM8
	mov r0, #0x08										;@ Compiled for ARM8 CPU
.endif
	orr r0, r0, #(4 << 5)								;@ Code is setup to support 4 cores	
.if (__ARM_FP == 12)
	orr r0, #0x80000000									;@ Set the hard float flag
.endif
	ldr r1, =RPi_CompileMode
	str r0, [r1]										;@ Store the compiler mode  
//"========================================================================="
//#        Try Auto-Detect Raspberry PI USB vendor ID at Pi1 position
//#	    LOGIC: All models of Pi have USB as Ethernet/wifi sit off it
//"========================================================================="
	ldr r2, =0x20000000									;@ Default Pi address as 0x20000000
	ldr r3, =0x40000000									;@ Default Pi ARM to GPU alias address as 0x40000000
	ldr r1, =0x4F54280A									;@ This is the Vendor ID a PI1 has
	ldr r0, =0x20980040									;@ USB vendor ID address on a Pi1
	ldr r0, [r0]										;@ Fetch value at vendor ID
	cmp r1, r0											;@ Check if we have the correct vendor ID
	beq .Is_a_Pi1
	ldr r2, =0x3F000000									;@ Change Pi address to Pi2/3 at 0x3F000000
	ldr r3, =0xC0000000									;@ Change Pi ARM to GPU alias address as 0xC0000000
.Is_a_Pi1:
//"========================================================================="
//#		 	   Sets RPi_IO_Base_Addr & RPi_ARM_TO_GPU_Alias
//"========================================================================="
	ldr r1, =RPi_IO_Base_Addr							;@ Address to RPi_IO_Base_Addr
	str r2, [r1]										;@ Save RPi_IO_Base_Addr for interface
	ldr r1, =RPi_ARM_TO_GPU_Alias						;@ Address to RPi_ARM_TO_GPU_Alias
	str r3, [r1]										;@ Save RPi_ARM_TO_GPU_Alias for interface
;@"========================================================================="
@#     We are getting close to handing over to C so we need to copy the 
@#     ISR table to position 0x0000 so interrupts can be used if wanted 
;@"========================================================================="
	ldr r0, = _vectors									;@ Address of isr_Table
	mov     r1, #0x0000									;@ Destination 0x0000
    ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8, r9}
    stmia   r1!,{r2, r3, r4, r5, r6, r7, r8, r9}
    ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8, r9}
    stmia   r1!,{r2, r3, r4, r5, r6, r7, r8, r9}
;@"========================================================================="
@#    Bring Core 1,2,3 to secondary spin if not an ARM6 and bootloaded
@#    This will setup stacks on core1,2,3 and drop them from hyp etc
;@"========================================================================="
	mrc p15,0,r0,c0,c0,0								;@ Read Main ID Register
	ldr r1, =ARM6_CPU_ID								;@ fetch ARM6_CPU_ID
	cmp r1, r0											;@ Check for match
	beq .NoMultiCoreSetup								;@ ARM6 jumps as no multicore
	ldr r1, =RPi_BootAddr								;@ Address to hold Boot address
	ldr r0, [r1]										;@ Fetch bootAddress
	ldr r1, =0x8000										;@ Check against 0x8000
	cmp r1, r0											;@ Check for match	
	bne .NoMultiCoreSetup								;@ Not bootladed so jump multicore setup
 	mov	r1, #1073741824									;@ 0x40000000
    ldr r2, =_start										;@ Address of CoreSetup call
	str	r2, [r1, #156]									;@ Write ExtraCoreSetup to 0x4000009C which signals core1 to jump
 	sev													;@ Wake core up
	ldr r3, =RPi_CoresReady								;@ Address of CoreReady count
.WaitCore1ACK:
	ldr	r1, [r3]										;@ Read CoreReady
	cmp	r1, #2											;@ Wait for setting of second core ready
	bne	.WaitCore1ACK									;@ Core1 not ready so read again
 	mov	r1, #1073741824									;@ 0x40000000
    ldr r2, =_start										;@ Address of CoreSetup call
	str	r2, [r1, #172]									;@ Write ExtraCoreSetup to 0x400000AC which signals core2 to jump
 	sev													;@ Wake core up
.WaitCore2ACK:
	ldr	r1, [r3]										;@ Read CoreReady count
	cmp	r1, #3											;@ Wait for setting of third core ready
	bne	.WaitCore2ACK									;@ Core2 not ready so read again
 	mov	r1, #1073741824									;@ 0x40000000
    ldr r2, =_start										;@ Address of CoreSetup call
	str	r2, [r1, #188]									;@ Write ExtraCoreSetup to 0x400000BC which signals core3 to jump
 	sev													;@ Wake core up
.WaitCore3ACK:
	ldr	r1, [r3]										;@ Read CoreReady count
	cmp	r1, #4											;@ check for ACK
	bne	.WaitCore3ACK									;@ Core3 not ready so read again
.NoMultiCoreSetup:

	/* Branch to the platform-independent C startup code, which takes no
	 * parameters and never returns.  */
	b nulluser
hang:
	b hang												;@ Hang if it returns from main call
.balign	4
.ltorg													;@ Tell assembler ltorg data for this code can go here

/* ARM exception vector table.  This is copied to address 0.  See A2.6
 * "Exceptions" of the ARM Architecture Reference Manual.  */
_vectors:
	ldr pc, reset_addr    /* Reset handler */
	ldr pc, undef_addr	  /* Undefined instruction handler */
	ldr pc, swi_addr	  /* Software interrupt handler */
	ldr pc, prefetch_addr /* Prefetch abort handler */
	ldr pc, abort_addr	  /* Data abort handler */
	ldr pc, reserved_addr /* Reserved */
	ldr pc, irq_addr	  /* IRQ (Interrupt request) handler */
	ldr pc, fiq_addr	  /* FIQ (Fast interrupt request) handler */

reset_addr:     .word _start
undef_addr:     .word hang
swi_addr:       .word hang
prefetch_addr:  .word hang
abort_addr:     .word hang
reserved_addr:  .word hang
irq_addr:       .word irq_handler
fiq_addr:       .word hang

;@"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
;@    Modified bootloader Spin loop but tolerant on registers R0-R3 for C   
;@"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
.balign	4
SecondarySpin:
	mrc     p15, 0, r0, c0, c0, 5						;@ Read Ccore ID from CPU register
	ands r0, r0, #0x3									;@ Make core 2 bit bitmask in R0
    ldr r5, =mbox										;@ Load mailbox read address
	ldr r5, [r5]										;@ Load mailbox address
	mov	r3, #0											;@ Magic compare to zero
SecondarySpinLoop:
	wfe													;@ Wait for event .. sleeps core
	ldr	r4, [r5, r0, lsl #4]							;@ Read the mailbox				
	cmp	r4, r3											;@ compare to zero
	beq	SecondarySpinLoop								;@ if zero reloop
	str	r4, [r5, r0, lsl #4]							;@ Clear the read address
	mov	r0, #0											;@ Zero R0
    ldr r1, =machid										;@ Address of Machine ID value
	ldr r1, [r1]										;@ Load machine id to r1
	ldr r2, =atags										;@ Adress to atags value									
	ldr r2, [r2]										;@ Fetch ATAGS value
	ldr lr, =SecondarySpin								;@ Set link register to secondary spin address
	bx	r4												;@ Call adress we loaded in r4
	b SecondarySpin										;@ Branch to secondary spin
mbox: 	.4byte 0x400000CC
machid:	.4byte 3138
atags:  .4byte 0x100
.balign	4
.ltorg													;@ Tell assembler ltorg data for this code can go here

.if (__ARM_FP == 12)
.balign	4
.globl get_fpexc
.type get_fpexc, %function
get_fpexc:
	fmrx r0, fpexc
	bx  lr												;@ Return

.balign	4
.globl get_fpscr
.type get_fpscr, %function
get_fpscr:
	fmrx r0, fpscr
	bx  lr												;@ Return
.endif

;@"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
@#     	          DATA FOR SMARTSTART32 EXPOSED TO INTERFACE 
;@"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
.section ".data.smartstart32", "aw"
.balign 4

.globl RPi_IO_Base_Addr;								;@ Make sure Pi_IO_Base_Addr label is global
RPi_IO_Base_Addr : .4byte 0;							;@ Peripheral Base addr is 4 byte variable in 32bit mode

.globl RPi_ARM_TO_GPU_Alias;							;@ Make sure RPi_ARM_TO_GPU_Alias label is global
RPi_ARM_TO_GPU_Alias: .4byte 0;							;@ ARM to GPU alias is 4 byte variable in 32bit mode

.globl RPi_BootAddr;									;@ Make sure RPi_BootAddr label is global
RPi_BootAddr : .4byte 0;								;@ CPU boot address is 4 byte variable in 32bit mode

.globl RPi_CoresReady;									;@ Make sure RPi_CoresReady label is global
RPi_CoresReady : .4byte 0;								;@ CPU cores ready for use is 4 byte variable in 32bit mode

.globl RPi_CPUBootMode;									;@ Make sure RPi_CPUBootMode label is global
RPi_CPUBootMode : .4byte 0;								;@ CPU Boot Mode is 4 byte variable in 32bit mode

.globl RPi_CpuId;										;@ Make sure RPi_CpuId label is global
RPi_CpuId : .4byte 0;									;@ CPU Id is 4 byte variable in 32bit mode

.globl RPi_CompileMode;									;@ Make sure RPi_CompileMode label is global
RPi_CompileMode : .4byte 0;								;@ Compile mode is 4 byte variable in 32bit mode

.globl RPi_CPUCurrentMode;								;@ Make sure RPi_CPUCurrentMode label is global
RPi_CPUCurrentMode : .4byte 0;							;@ CPU current Mode is 4 byte variable in 32bit mode

.globl RPi_SmartStartVer;								;@ Make sure RPi_SmartStartVer label is global
RPi_SmartStartVer : .4byte 0x00020007;					;@ SmartStart version is 4 byte variable in 32bit mode