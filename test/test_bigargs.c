#include <stddef.h>
#include <thread.h>
#include <stdio.h>
#include <testsuite.h>

static void bigargs(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned char e, unsigned char f,
                    unsigned char *testArray)
{
    testArray[0] = a;
    testArray[1] = b;
    testArray[2] = c;
    testArray[3] = d;
    testArray[4] = e;
    testArray[5] = f;
    testArray[6] = a + b + c + d + e + f;
    return;
}

thread test_bigargs(bool verbose)
{
    unsigned char testArray[20];

    ready(create((void *)bigargs, INITSTK, 31, "BIGARGS",
                 7, 10, 20, 30, 40, 50, 60, testArray));
	resched();
    if ((10 == testArray[0])
        && (20 == testArray[1])
        && (30 == testArray[2])
        && (40 == testArray[3])
        && (50 == testArray[4])
        && (60 == testArray[5]) && (210 == testArray[6]))
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    return OK;
}
