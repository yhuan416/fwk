#include "t_osal_task.h"

static int i = 0;

static void *taskEntry(void *arg)
{
    CU_ASSERT_EQUAL(i, 0);
    i++;
    return NULL;
}

static void task_01(void)
{
    CU_ASSERT_EQUAL(i, 0);

    osal_task_attr_t task_attr = {0};
    task_attr.name = "task1";

    osal_task_t task = osal_task_create(
        (osal_task_func_t)taskEntry,
        NULL,
        &task_attr);
    CU_ASSERT_PTR_NOT_NULL(task);

    osal_task_sleep(1);
    CU_ASSERT_EQUAL(i, 1);
}

void t_osal_task_001(void)
{
    CU_add_test(task_suite, "task_01", task_01);
}
