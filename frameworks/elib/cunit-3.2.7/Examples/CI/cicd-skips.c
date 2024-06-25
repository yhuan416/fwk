/**
 * Simple CICD example to demonstrate skipping tests
 *
 *  CUnit - A Unit testing framework library for C.
 *  Copyright (C) 2001       Anil Kumar
 *  Copyright (C) 2004-2006  Anil Kumar, Jerry St.Clair
 *  Copyright (C) 2018       Ian Norton
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 */

#include "CUnit/CUnitCI.h"


static int test_setup_count = 0;

static void test_simple_pass1(void) {
    CU_ASSERT_FATAL(1 == 1);
}

static void test_skipped(void) {
    CU_SKIP_IF(1 == 1);
    CU_ASSERT_FATAL(1 != 1);
}

static void test_simple_pass2(void) {
    CU_ASSERT_FATAL(1 == 1);
}

static void test_simple_never_run1(void) {
    /* this test is never executed because the setup function will skip after 3 uses */
    CU_ASSERT_FATAL(1 == 2);
}

static void test_simple_never_run2(void) {
    /* this test is never executed because the setup function will skip after 3 uses */
    CU_ASSERT_FATAL(1 == 2);
}

/* run at the start of each test */
CU_TEST_SETUP() {
    CU_SKIP_IF(test_setup_count++ > 2);
}

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(test_simple_pass1),
             CUNIT_CI_TEST(test_skipped),
             CUNIT_CI_TEST(test_simple_pass2),
             CUNIT_CI_TEST(test_simple_never_run1),
             CUNIT_CI_TEST(test_simple_never_run2)
);
