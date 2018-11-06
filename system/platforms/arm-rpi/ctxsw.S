/**
 * @file ctxsw.S
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <arm.h>

.globl ctxsw

/*------------------------------------------------------------------------
 *  ctxsw  -  Switch from one thread context to another.
 *------------------------------------------------------------------------
 *
 * This is the ARM version.  How it works: we have to save r4-r11 and lr, since
 * r4-r11 are callee-save and lr needs to be loaded into the pc when this
 * context is switched to again.  Registers r0-r3 are caller-save so they do
 * *not* need not be saved, but they are pushed anyway to leave space since they
 * are part of the context constructed by create() to pass thread arguments.
 *
 * When restoring a context, we pop both the lr and pc.  These are both set to
 * appropriate values in create().  But when saving a context below, we only
 * have an appropriate value for pc--- namely, the lr, a.k.a. the address
 * ctxsw() will return to.  The lr at that instruction is unknown.  However,
 * this is irrelevant because the lr is caller-save, and we can simply push a
 * garbage value from r13 instead.
 *
 * We almost don not need to do anything about the CPSR here, since:
 *
 *   - We do all our context switches to/from the same mode (namely, SYS mode).
 *   - The ARM ABI does not expect comparison flags in the CPSR to be preserved
 *         across function calls.
 *   - resched() takes care of saving/restoring whether interrupts are enabled
 *         or not when resuming a thread that has been switched out.
 *   - Xinu never makes changes to the CPSR not already covered above, such as
 *         switching to executing Thumb instructions.
 *
 * However, interrupts are disabled when ctxsw() is called from resched(), but
 * we want interrupts to be enabled when starting a *new* thread, which
 * resched() does not take care of.  We solve this by including the control bits
 * of the current program status register in the context and adding a line of
 * code to create() that sets the control bits of new threads such that
 * interrupts are enabled.
 *------------------------------------------------------------------------*/
/* C code call is ctxsw(&throld->stkptr, &thrnew->stkptr, asid) */
/* R0 = thread stack switching away from, R1 = new thread stack we ars switching to, r2 = asid  */
ctxsw:
	.func ctxsw
	mrs r12, cpsr
	push {r14}
	push {r13}
	push {r0-r12}

.if (__ARM_FP == 12)
	/* If compiler has hard floats on we will need to save fpu registers */

	/* Save the Floating point registers D0 to D15 onto the old stack */
	fstmdbd sp!, {d0-d15}
 
	/* Save the Floating point FPSCR onto the old stack */
	fmrx r12, fpscr
	push {r12}

	/* Save the Floating point FPEXC onto the old stack */
	fmrx r12, fpexc
	push {r12}
.endif

	str sp, [r0]
	ldr sp, [r1]

.if (__ARM_FP == 12)
	/* If compiler has hard floats on we will need to restore fpu registers */

	/* Restore the Floating point FPEXC from the new stack */
	pop {r12}
	fmxr fpexc, r12
 
	/* Restore the Floating point FPSCR from the new stack */
	pop {r12}
	fmxr fpscr, r12
  
	/* Restore the Floating point registers D0 to D15 from the new stack */
	fldmiad sp!, {d0-d15}
.endif

	pop {r0-r12}
	msr cpsr_c, r12
	pop {lr, pc}
	.endfunc
