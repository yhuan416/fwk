#include "common.h"

/* run at the start of each suite */
CU_SUITE_SETUP()
{
    return CUE_SUCCESS;
}

/* run at the end of the suite */
CU_SUITE_TEARDOWN()
{
    return CUE_SUCCESS;
}

/* run at the start of each test */
CU_TEST_SETUP()
{
}

/* run at the end of each test */
CU_TEST_TEARDOWN()
{
}

int i = 0;
osal_task_t task;

void *taskEntry(void *arg)
{
    CU_ASSERT_PTR_EQUAL(task, osal_task_self());

    CU_ASSERT_EQUAL(i, 0);

    i++;

    CU_ASSERT_EQUAL(i, 1);

    return NULL;
}

static void task1(void)
{
    CU_ASSERT_EQUAL(i, 0);

    task = osal_task_create(
        "task1",
        (osal_task_func_t)taskEntry,
        NULL,
        NULL,
        0,
        0);
    CU_ASSERT_PTR_NOT_NULL(task);

    osal_task_sleep(1);

    CU_ASSERT_EQUAL(i, 1);
}

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(task1));
