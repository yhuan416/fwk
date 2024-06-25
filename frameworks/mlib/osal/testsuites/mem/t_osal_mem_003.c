#include "t_osal_mem.h"

static void realloc_01(void)
{
    void *buff = osal_malloc(1024);
    CU_ASSERT_PTR_NOT_NULL(buff);

    buff = osal_realloc(buff, 2048);
    CU_ASSERT_PTR_NOT_NULL(buff);

    osal_free(buff);
}

static void realloc_02(void)
{
    void *buff = NULL;

    buff = osal_realloc(buff, 2048); // same as malloc
    CU_ASSERT_PTR_NOT_NULL(buff);

    void *buff1 = NULL;

    buff1 = osal_realloc(NULL, 1024); // same as malloc
    CU_ASSERT_PTR_NOT_NULL(buff1);

    osal_free(buff);
    osal_free(buff1);
}

static void realloc_03(void)
{
    void *buff = osal_malloc(1024);
    CU_ASSERT_PTR_NOT_NULL(buff);

    buff = osal_realloc(buff, 0); // same as free
    CU_ASSERT_PTR_NULL(buff);

    osal_free(buff);
}

void t_osal_mem_003(void)
{
    CU_add_test(mem_suite, "realloc_01", realloc_01);
    CU_add_test(mem_suite, "realloc_02", realloc_02);
    CU_add_test(mem_suite, "realloc_03", realloc_03);
}
