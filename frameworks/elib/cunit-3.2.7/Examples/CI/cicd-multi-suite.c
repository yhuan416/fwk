/**
 * A single test program that groups tests into several suites
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

static void test_one(void) {
    CU_ASSERT_FATAL(1);
}

static void test_two(void) {
    CU_ASSERT_FATAL(1);
}


int main(int argc, char** argv) {
    CUNIT_CI_CLEAR_SETUPS();
    CU_CI_DEFINE_SUITE("first suite", 0, 0, 0, 0);
    CUNIT_CI_TEST(test_one);

    CU_CI_DEFINE_SUITE("second suite", 0, 0, 0, 0);
    CUNIT_CI_TEST(test_two);

    return CU_CI_RUN_SUITES();
}