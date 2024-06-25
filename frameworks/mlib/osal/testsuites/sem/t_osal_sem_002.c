#include "t_osal_sem.h"

static void sem_02(void)
{
    osal_sem_t sem1;
    osal_sem_t sem2;

    // 创建sem1
    sem1 = osal_sem_create(10, 0);
    CU_ASSERT_PTR_NOT_NULL(sem1);

    // 尝试获取
    CU_ASSERT_NOT_EQUAL(osal_sem_wait(sem1, 1000), OSAL_API_OK);

    // 销毁
    CU_ASSERT_EQUAL(osal_sem_destroy(sem1), OSAL_API_OK);
    sem1 = NULL;

    // 创建sem2
    sem2 = osal_sem_create(10, 1);
    CU_ASSERT_PTR_NOT_NULL(sem2);

    // 获取
    CU_ASSERT_EQUAL(osal_sem_wait(sem2, 1000), OSAL_API_OK);
    CU_ASSERT_NOT_EQUAL(osal_sem_wait(sem2, 1000), OSAL_API_OK);

    // 销毁
    CU_ASSERT_EQUAL(osal_sem_destroy(sem2), OSAL_API_OK);
    sem2 = NULL;
}

void t_osal_sem_002(void)
{
    CU_add_test(sem_suite, "sem_02", sem_02);
}
