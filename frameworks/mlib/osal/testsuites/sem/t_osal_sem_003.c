#include "t_osal_sem.h"

void *taskEntry(void *arg)
{
    osal_sem_t sem1 = (osal_sem_t)arg;

    CU_ASSERT_EQUAL(osal_sem_post(sem1), OSAL_API_OK);
    osal_task_sleep(1);
    CU_ASSERT_EQUAL(osal_sem_post(sem1), OSAL_API_OK);

    return NULL;
}

static void sem_03(void)
{
    osal_task_attr_t task_attr = {0};

    osal_sem_t sem1 = osal_sem_create(10, 0);
    CU_ASSERT_PTR_NOT_NULL(sem1);

    task_attr.name = "task1";

    osal_task_t task = osal_task_create(
        (osal_task_func_t)taskEntry,
        (void *)sem1,
        &task_attr);
    CU_ASSERT_PTR_NOT_NULL(task);

    CU_ASSERT_EQUAL(osal_sem_wait(sem1, OSAL_WAITFOREVER), OSAL_API_OK);
    CU_ASSERT_EQUAL(osal_sem_wait(sem1, OSAL_WAITFOREVER), OSAL_API_OK);
    CU_ASSERT_EQUAL(osal_sem_destroy(sem1), OSAL_API_OK);
}

void t_osal_sem_003(void)
{
    CU_add_test(sem_suite, "sem_03", sem_03);
}
