/**
 * Simple CICD example to demonstrate skipping suites
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


static void test_simple_never_run1(void) {
    /* this test is never executed because the suite will be skipped */
    CU_ASSERT_FATAL(1 == 2);
}

static void test_simple_never_run2(void) {
    /* this test is never executed because the suite will be skipped */
    CU_ASSERT_FATAL(1 == 2);
}

CU_SUITE_SETUP() {
    CU_SKIP_IF(1 > 0);
    return CUE_SUCCESS;
}

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(test_simple_never_run1),
             CUNIT_CI_TEST(test_simple_never_run2)
);
