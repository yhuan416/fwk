#include "t_osal_sem.h"

#include <unistd.h>

static void sem_01(void)
{
    osal_sem_t sem1;
    osal_sem_t sem2;

    sem1 = osal_sem_create(10, 0);
    CU_ASSERT_PTR_NOT_NULL(sem1);

    sem2 = osal_sem_create(10, 1);
    CU_ASSERT_PTR_NOT_NULL(sem2);

    CU_ASSERT_EQUAL(osal_sem_destroy(sem1), OSAL_API_OK);
    CU_ASSERT_EQUAL(osal_sem_destroy(sem2), OSAL_API_OK);
}

void t_osal_sem_001(void)
{
    CU_add_test(sem_suite, "sem_01", sem_01);
}
