#include "t_osal_mem.h"

static void calloc_01(void)
{
    void *buff = osal_calloc(1, 1024);
    CU_ASSERT_PTR_NOT_NULL(buff);
    osal_free(buff);
}

static void calloc_02(void)
{
    void *buff = osal_calloc(0, 1024);
    CU_ASSERT_PTR_NULL(buff);
    osal_free(buff);
}

static void calloc_03(void)
{
    void *buff = osal_calloc(1, 0);
    CU_ASSERT_PTR_NULL(buff);
    osal_free(buff);
}

static void calloc_04(void)
{
    void *buff = osal_calloc(0, 0);
    CU_ASSERT_PTR_NULL(buff);
    osal_free(buff);
}

void t_osal_mem_002(void)
{
    CU_add_test(mem_suite, "calloc_01", calloc_01);
    CU_add_test(mem_suite, "calloc_02", calloc_02);
    CU_add_test(mem_suite, "calloc_03", calloc_03);
    CU_add_test(mem_suite, "calloc_04", calloc_04);
}
