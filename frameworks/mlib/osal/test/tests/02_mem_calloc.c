#include "common.h"

/* run at the start of each suite */
CU_SUITE_SETUP() {
    return CUE_SUCCESS;
}

/* run at the end of the suite */
CU_SUITE_TEARDOWN() {
    return CUE_SUCCESS;
}

/* run at the start of each test */
CU_TEST_SETUP() {
    
}

/* run at the end of each test */
CU_TEST_TEARDOWN() {

}

static void mem_calloc1(void)
{
    void *buff = osal_calloc(1024, 1);
    CU_ASSERT_PTR_NOT_NULL(buff);
    osal_free(buff);
}

static void mem_calloc2(void)
{
    void *buff = osal_calloc(0, 1);
    CU_ASSERT_PTR_NULL(buff);
    osal_free(buff);
}

static void mem_calloc3(void)
{
    void *buff = osal_calloc(1024, 0);
    CU_ASSERT_PTR_NULL(buff);
    osal_free(buff);
}

CUNIT_CI_RUN("mem_calloc",
             CUNIT_CI_TEST(mem_calloc1),
             CUNIT_CI_TEST(mem_calloc2),
             CUNIT_CI_TEST(mem_calloc3)
);
