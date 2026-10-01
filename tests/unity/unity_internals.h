/* ==========================================
    Unity Project - A Test Framework for C
    [MIT License]
========================================== */
#ifndef UNITY_INTERNALS_H
#define UNITY_INTERNALS_H

#include <stdio.h>
#include <setjmp.h>

typedef void (*UnityTestFunction)(void);

typedef struct {
    const char *TestFile;
    const char *CurrentTestName;
    uint32_t CurrentTestLineNumber;
    uint32_t NumberOfTests;
    uint32_t TestFailures;
    uint32_t TestIgnores;
    jmp_buf AbortFrame;
} Unity_t;

extern Unity_t Unity;

void UnityBegin(const char *filename);
int  UnityEnd(void);
void UnityDefaultTestRun(UnityTestFunction Func, const char *FuncName, const int FuncLineNum);
void UnityFail(const char *msg, const int line);

#define UNITY_TEST_ASSERT(condition, line, msg) if (!(condition)) { UnityFail((msg), (line)); }
#define UNITY_TEST_ASSERT_NULL(pointer, line, msg) if ((pointer) != NULL) { UnityFail((msg), (line)); }
#define UNITY_TEST_ASSERT_NOT_NULL(pointer, line, msg) if ((pointer) == NULL) { UnityFail((msg), (line)); }
#define UNITY_TEST_ASSERT_EQUAL_INT(expected, actual, line, msg) if ((expected) != (actual)) { char _buf[128]; snprintf(_buf, sizeof(_buf), "%s: Expected %d Was %d", (msg), (int)(expected), (int)(actual)); UnityFail(_buf, (line)); }
#define UNITY_TEST_ASSERT_EQUAL_UINT(expected, actual, line, msg) if ((expected) != (actual)) { char _buf[128]; snprintf(_buf, sizeof(_buf), "%s: Expected %u Was %u", (msg), (unsigned int)(expected), (unsigned int)(actual)); UnityFail(_buf, (line)); }
#define UNITY_TEST_ASSERT_EQUAL_HEX16(expected, actual, line, msg) if ((expected) != (actual)) { char _buf[128]; snprintf(_buf, sizeof(_buf), "%s: Expected 0x%04X Was 0x%04X", (msg), (unsigned int)(expected), (unsigned int)(actual)); UnityFail(_buf, (line)); }
#define UNITY_TEST_ASSERT_EQUAL_STRING(expected, actual, line, msg) if (strcmp((expected), (actual)) != 0) { char _buf[128]; snprintf(_buf, sizeof(_buf), "%s: Expected '%s' Was '%s'", (msg), (expected), (actual)); UnityFail(_buf, (line)); }
#define UNITY_TEST_ASSERT_INT_WITHIN(delta, expected, actual, line, msg) { int diff = (int)(expected) - (int)(actual); if (diff < 0) diff = -diff; if (diff > (int)(delta)) { char _buf[128]; snprintf(_buf, sizeof(_buf), "%s: Expected within %d of %d Was %d", (msg), (int)(delta), (int)(expected), (int)(actual)); UnityFail(_buf, (line)); } }

#endif /* UNITY_INTERNALS_H */
