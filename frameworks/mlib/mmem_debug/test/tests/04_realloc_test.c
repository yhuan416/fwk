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

static void realloc_test1(void)
{
    char *buffer = malloc(1024);
    CU_ASSERT_PTR_NOT_NULL(buffer);
    buffer = realloc(buffer, 2048);
    CU_ASSERT_PTR_NOT_NULL(buffer);
    free(buffer);
}

static void realloc_test2_realloc_addr_null(void)
{
    CU_ASSERT_PTR_NOT_NULL(realloc(NULL, 2048));
}

static void realloc_test3_realloc_size_0(void)
{
    char *buffer = malloc(1024);
    CU_ASSERT_PTR_NOT_NULL(buffer);
    buffer = realloc(buffer, 0);
    CU_ASSERT_PTR_NULL(buffer);
}

static void realloc_test4_realloc_addr_null_size_0(void)
{
    CU_ASSERT_PTR_NULL(realloc(NULL, 0));
}

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(realloc_test1),
             CUNIT_CI_TEST(realloc_test2_realloc_addr_null),
             CUNIT_CI_TEST(realloc_test3_realloc_size_0),
             CUNIT_CI_TEST(realloc_test4_realloc_addr_null_size_0)
);
