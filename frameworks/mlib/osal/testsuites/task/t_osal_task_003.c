#include "t_osal_task.h"

static int i = 0;

static void *taskEntry(void *arg)
{
    CU_ASSERT_EQUAL(i, 0);
    i++;
    return NULL;
}

static void task_03(void)
{
    CU_ASSERT_EQUAL(i, 0);

    osal_task_attr_t task_attr = {0};
    task_attr.name = NULL;

    osal_task_t task1 = osal_task_create(
        (osal_task_func_t)taskEntry,
        NULL,
        &task_attr);
    CU_ASSERT_PTR_NOT_NULL(task1); // name可以为空, 返回task句柄

    osal_task_sleep(1);
    CU_ASSERT_EQUAL(i, 1); // task执行, i加1
}

void t_osal_task_003(void)
{
    CU_add_test(task_suite, "task_03", task_03);
}
