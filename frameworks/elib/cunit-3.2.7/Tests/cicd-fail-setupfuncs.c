/**
 * Test CICD/Junit with setup and teardown functions failing
 *
 * This test forks a copy of itself for each failure mode
 *
 *  CUnit - A Unit testing framework library for C.
 *  Copyright (C) 2001       Anil Kumar
 *  Copyright (C) 2004-2006  Anil Kumar, Jerry St.Clair
 *  Copyright (C) 2018-2020  Ian Norton
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 */
#include "CUnit/AutomatedJUnitXml.h"
#include "CUnit/CUnitCI.h"

static int suite_setup_count = 0;
static int suite_teardown_count = 0;
static int test_setup_count = 0;
static int test_teardown_count = 0;

static int fail_suite_setup = 0;
static int fail_suite_cleanup = 0;
static int fail_test_setup = 0;
static int fail_test_teardown = 0;
static int fail_test_func = 0;
static int child_test = 0;

static int is_env_set(const char* name) {
  const char * value = getenv(name);
  if (value) {
    return value[0] == '1';
  }
  return 0;
}

static void reset_environment(void) {
  putenv("CU_FAIL_SUITE_SETUP=0");
  putenv("CU_FAIL_SUITE_CLEANUP=0");
  putenv("CU_FAIL_TEST_SETUP=0");
  putenv("CU_FAIL_TEST_TEARDOWN=0");
  putenv("CU_FAIL_TEST=0");
}

static void before_suite_setup(void) {
  if (!child_test){
    reset_environment();
  }
  fail_suite_setup = is_env_set("CU_FAIL_SUITE_SETUP");
  fail_suite_cleanup = is_env_set("CU_FAIL_SUITE_CLEANUP");
  fail_test_setup = is_env_set("CU_FAIL_TEST_SETUP");
  fail_test_teardown = is_env_set("CU_FAIL_TEST_TEARDOWN");
  fail_test_func = is_env_set("CU_FAIL_TEST");
}

/* run at the start of each suite */
CU_SUITE_SETUP() {
    before_suite_setup();
    if (child_test) {
      fprintf(stdout, "\n+++ setting up suite\n");
      suite_setup_count++;
      if (fail_suite_setup) return CUE_SINIT_FAILED;
    }
    return CUE_SUCCESS;
}

/* run at the end of the suite */
CU_SUITE_TEARDOWN() {
    if (child_test) {
      fprintf(stdout, "\n+++ cleaning up suite\n");
      suite_teardown_count++;
      if (fail_suite_cleanup) return CUE_SINIT_FAILED;
    }
    return CUE_SUCCESS;
}

/* run at the start of each test */
CU_TEST_SETUP() {
    if (!child_test) {
      reset_environment();
    } else {
      fprintf(stdout, "\n++++ test setup\n");
      test_setup_count++;
      CU_ASSERT_FATAL(!fail_test_setup);
    }
}

/* run at the end of each test */
CU_TEST_TEARDOWN() {
    if (child_test) {
      fprintf(stdout, "\n++++ test teardown\n");
      test_teardown_count++;
      CU_ASSERT_FATAL(!fail_test_teardown);
    }
}

static void test_simple_pass1(void) {
    CU_ASSERT_FATAL(suite_setup_count == 1);
    CU_ASSERT_FATAL(suite_teardown_count == 0);
    CU_ASSERT_FATAL(test_setup_count == 1);
    CU_ASSERT_FATAL(test_teardown_count == 0);
}

static void test_simple_pass2(void) {
    CU_ASSERT_FATAL(suite_setup_count == 1);
    CU_ASSERT_FATAL(suite_teardown_count == 0);
    CU_ASSERT_FATAL(test_setup_count == 2);
    CU_ASSERT_FATAL(test_teardown_count == 1);
}

static void test_simple_fail1(void) {
    if (fail_test_func) {
      fprintf(stdout, "intentionally failing test\n");
      CU_ASSERT_FATAL(!fail_test_func);
    } else {
      fprintf(stdout, "not intentionally failing test\n");
    }
}

static int execute_child_test(const char* new_report) {
  int argc;
  char **argv;
  const char* report_file = CU_automated_get_junit_filename();
  int rv = 0;
  CU_CI_args(&argc, &argv);
  putenv("CU_TEST_CHILD=1");
  fprintf(stdout, "Running child test for %s:\n", new_report);
  rv = system(argv[0]);
  /* move the test result file */
  remove(new_report); /* might not exist */
  fprintf(stdout, "Renaming child report from %s to %s\n", report_file, new_report);
  rename(report_file, new_report);
  return rv;
}

/* test children with failing elements */
static void test_expected_suite_setup_error(void) {
  putenv("CU_FAIL_SUITE_SETUP=1");
  CU_ASSERT_NOT_EQUAL(execute_child_test("expected-suite-setup-failed.xml"), 0);
}

static void test_expected_suite_cleanup_error(void) {
  putenv("CU_FAIL_SUITE_CLEANUP=1");
  CU_ASSERT_NOT_EQUAL(execute_child_test("expected-suite-cleanup-failed.xml"), 0);
}

static void test_expected_test_setup_error(void) {
  putenv("CU_FAIL_TEST_SETUP=1");
  CU_ASSERT_NOT_EQUAL(execute_child_test("expected-test-setup-failed.xml"), 0);
}

static void test_expected_test_teardown_error(void) {
  putenv("CU_FAIL_TEST_TEARDOWN=1");
  CU_ASSERT_NOT_EQUAL(execute_child_test("expected-test-teardown-failed.xml"), 0);
}

static void test_expected_test_error(void) {
  putenv("CU_FAIL_TEST=1");
  CU_ASSERT_NOT_EQUAL(execute_child_test("expected-test-failed.xml"), 0);
}

int main(int argc, char** argv) {
  child_test = is_env_set("CU_TEST_CHILD");
  CU_CI_add_suite(CU_MAIN_EXE_NAME,
            __cu_suite_setup,
            __cu_suite_teardown,
            __cu_test_setup,
            __cu_test_teardown);
  if (!child_test) {
    CUNIT_CI_TEST(test_expected_suite_setup_error);
    CUNIT_CI_TEST(test_expected_suite_cleanup_error);
    CUNIT_CI_TEST(test_expected_test_setup_error);
    CUNIT_CI_TEST(test_expected_test_teardown_error);
    CUNIT_CI_TEST(test_expected_test_error);
  } else {
    CUNIT_CI_TEST(test_simple_pass1);
    CUNIT_CI_TEST(test_simple_pass2);
    CUNIT_CI_TEST(test_simple_fail1);
  }
  return CU_CI_main(argc, argv);
}
