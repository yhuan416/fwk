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

static void malloc_test1_malloc_1024(void)
{
    CU_ASSERT_PTR_NOT_NULL(malloc(1024));
}

static void malloc_test2_malloc_0(void)
{
    CU_ASSERT_PTR_NULL(malloc(0));
}

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(malloc_test1_malloc_1024),
             CUNIT_CI_TEST(malloc_test2_malloc_0)
);
