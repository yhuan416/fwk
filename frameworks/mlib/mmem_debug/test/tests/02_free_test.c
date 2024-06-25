#include "common.h"

/* run at the start of each suite */
CU_SUITE_SETUP() {
    return CUE_SUCCESS;
}

/* run at the end of the suite */
CU_SUITE_TEARDOWN() {
    mmem_free_all();
    return CUE_SUCCESS;
}

/* run at the start of each test */
CU_TEST_SETUP() {

}

/* run at the end of each test */
CU_TEST_TEARDOWN() {

}

static void free_test1(void)
{
    char *buffer = malloc(1024);
    CU_ASSERT_PTR_NOT_NULL(buffer);
    free(buffer);
    CU_PASS("free(buffer) success");
}

static void free_test2_free_null(void)
{
    free(NULL);
    CU_PASS("free(NULL) success");
}

static void free_test3_double_free(void)
{
    char *buffer = malloc(1024);
    CU_ASSERT_PTR_NOT_NULL(buffer);
    free(buffer);
    free(buffer);
    CU_PASS("free(buffer) success");
}

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(free_test1),
             CUNIT_CI_TEST(free_test2_free_null),
             CUNIT_CI_TEST(free_test3_double_free)
);
