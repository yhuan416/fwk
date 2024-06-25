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

static void calloc_test1(void)
{
    CU_ASSERT_PTR_NOT_NULL(calloc(1, 1024));
}

static void calloc_test2_calloc_counts_0(void)
{
    CU_ASSERT_PTR_NULL(calloc(0, 1024));
}

static void calloc_test3_calloc_size_0(void)
{
    CU_ASSERT_PTR_NULL(calloc(1, 0));
}

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(calloc_test1),
             CUNIT_CI_TEST(calloc_test2_calloc_counts_0),
             CUNIT_CI_TEST(calloc_test3_calloc_size_0)
);
