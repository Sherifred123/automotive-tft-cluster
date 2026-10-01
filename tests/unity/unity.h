/* ==========================================
    Unity Project - A Test Framework for C
    Copyright (c) 2007-14 Mike Karlesky, Mark VanderVoord, Greg Williams
    [MIT License]
========================================== */
#ifndef UNITY_FRAMEWORK_H
#define UNITY_FRAMEWORK_H

#define UNITY_VERSION_MAJOR    2
#define UNITY_VERSION_MINOR    5
#define UNITY_VERSION_BUILD    2

#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "unity_internals.h"

#define TEST_ASSERT(condition)                               UNITY_TEST_ASSERT((condition), __LINE__, " Expression Evaluated To FALSE")
#define TEST_ASSERT_TRUE(condition)                          UNITY_TEST_ASSERT((condition), __LINE__, " Expected TRUE Was FALSE")
#define TEST_ASSERT_FALSE(condition)                         UNITY_TEST_ASSERT(!(condition), __LINE__, " Expected FALSE Was TRUE")
#define TEST_ASSERT_NULL(pointer)                            UNITY_TEST_ASSERT_NULL((pointer), __LINE__, " Expected NULL")
#define TEST_ASSERT_NOT_NULL(pointer)                        UNITY_TEST_ASSERT_NOT_NULL((pointer), __LINE__, " Expected Non-NULL")
#define TEST_ASSERT_EQUAL_INT(expected, actual)              UNITY_TEST_ASSERT_EQUAL_INT((expected), (actual), __LINE__, " Expected Values to be Equal")
#define TEST_ASSERT_EQUAL_INT16(expected, actual)            UNITY_TEST_ASSERT_EQUAL_INT((expected), (actual), __LINE__, " Expected Values to be Equal")
#define TEST_ASSERT_EQUAL_INT32(expected, actual)            UNITY_TEST_ASSERT_EQUAL_INT((expected), (actual), __LINE__, " Expected Values to be Equal")
#define TEST_ASSERT_EQUAL_UINT(expected, actual)             UNITY_TEST_ASSERT_EQUAL_UINT((expected), (actual), __LINE__, " Expected Values to be Equal")
#define TEST_ASSERT_EQUAL_UINT16(expected, actual)           UNITY_TEST_ASSERT_EQUAL_UINT((expected), (actual), __LINE__, " Expected Values to be Equal")
#define TEST_ASSERT_EQUAL_UINT32(expected, actual)           UNITY_TEST_ASSERT_EQUAL_UINT((expected), (actual), __LINE__, " Expected Values to be Equal")
#define TEST_ASSERT_EQUAL_HEX16(expected, actual)            UNITY_TEST_ASSERT_EQUAL_HEX16((expected), (actual), __LINE__, " Expected Hex Values to be Equal")
#define TEST_ASSERT_EQUAL_STRING(expected, actual)           UNITY_TEST_ASSERT_EQUAL_STRING((expected), (actual), __LINE__, " Expected Strings to be Equal")
#define TEST_ASSERT_INT_WITHIN(delta, expected, actual)      UNITY_TEST_ASSERT_INT_WITHIN((delta), (expected), (actual), __LINE__, " Expected Within Tolerance")

#define RUN_TEST(TestFunc) UnityDefaultTestRun(TestFunc, #TestFunc, __LINE__)

void setUp(void);
void tearDown(void);

#endif /* UNITY_FRAMEWORK_H */
