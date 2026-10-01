/* ==========================================
    Unity Project - A Test Framework for C
    [MIT License]
========================================== */
#include "unity.h"
#include <stdio.h>
#include <string.h>

Unity_t Unity;

void UnityBegin(const char *filename)
{
    Unity.TestFile = filename;
    Unity.CurrentTestName = NULL;
    Unity.CurrentTestLineNumber = 0;
    Unity.NumberOfTests = 0;
    Unity.TestFailures = 0;
    Unity.TestIgnores = 0;
}

int UnityEnd(void)
{
    printf("\n-----------------------\n");
    printf("%u Tests %u Failures %u Ignored\n", 
           (unsigned int)Unity.NumberOfTests, 
           (unsigned int)Unity.TestFailures, 
           (unsigned int)Unity.TestIgnores);
    if (Unity.TestFailures == 0) {
        printf("OK\n");
        return 0;
    } else {
        printf("FAIL\n");
        return (int)Unity.TestFailures;
    }
}

void UnityDefaultTestRun(UnityTestFunction Func, const char *FuncName, const int FuncLineNum)
{
    Unity.CurrentTestName = FuncName;
    Unity.CurrentTestLineNumber = (uint32_t)FuncLineNum;
    Unity.NumberOfTests++;

    if (setjmp(Unity.AbortFrame) == 0) {
        setUp();
        Func();
        tearDown();
        printf("PASS: %s\n", FuncName);
    } else {
        /* Test Failed */
    }
}

void UnityFail(const char *msg, const int line)
{
    Unity.TestFailures++;
    printf("FAIL: %s:%d: %s: %s\n", Unity.TestFile, line, Unity.CurrentTestName, msg);
    longjmp(Unity.AbortFrame, 1);
}
