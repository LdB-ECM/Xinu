#include <stdio.h>
#include <stddef.h>
#include <testsuite.h>
#include <interrupt.h>
#include <thread.h>

thread spin(void)
{
    volatile int n = 1;

    while (n != 0)
    {
    }

    return OK;
}

thread test_preempt(bool verbose)
{
    /* the failif macro depends on 'passed' and 'verbose' vars */
    bool passed = TRUE;
    tid_typ thrspin;

    /* This is the first "subtest" of this suite */
    thrspin =
        create(spin, INITSTK, thrtab[thrcurrent].prio, "test_spin", 0);

    /* Make spin ... spin */
    ready(thrspin);
	resched();
    /* If this next line runs, we're good */
    kill(thrspin);

    /* always print out the overall tests status */
    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    return OK;
}
