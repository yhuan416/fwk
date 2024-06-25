#include "t_osal_mq.h"

#include <unistd.h>

typedef struct msg
{
    uint32_t type;
} msg_t;

void *routine(void *arg)
{
    osal_mq_t mq = (osal_mq_t)arg;
    msg_t msg;
    msg.type = 1;
    CU_ASSERT_EQUAL(osal_mq_send(mq, &msg, sizeof(msg_t), 0), OSAL_API_OK);

    msg.type = 2;
    CU_ASSERT_EQUAL(osal_mq_send(mq, &msg, sizeof(msg_t), 1000), OSAL_API_OK);

    msg.type = 3;
    CU_ASSERT_EQUAL(osal_mq_send(mq, &msg, sizeof(msg_t), 2000), OSAL_API_OK);

    return NULL;
}

void *routine2(void *arg)
{
    msg_t msg;
    osal_mq_t mq = (osal_mq_t)arg;

    CU_ASSERT_NOT_EQUAL(osal_mq_recv(mq, &msg, sizeof(msg_t), OSAL_API_WAITFOREVER), OSAL_API_OK);

    return NULL;
}

static void mq_01(void)
{
    osal_task_attr_t task_attr = {0};
    osal_task_t task;
    osal_mq_t mq;

    mq = osal_mq_create("mq", sizeof(msg_t), 10, 0);
    CU_ASSERT_PTR_NOT_NULL(mq);

    task_attr.name = "mq";

    task = osal_task_create(routine, (void *)mq, &task_attr);
    CU_ASSERT_PTR_NOT_NULL(task);

    msg_t msg;
    CU_ASSERT_EQUAL(osal_mq_recv(mq, &msg, sizeof(msg_t), 1000), OSAL_API_OK);
    CU_ASSERT_EQUAL(msg.type, 1);

    CU_ASSERT_EQUAL(osal_mq_recv(mq, &msg, sizeof(msg_t), 1000), OSAL_API_OK);
    CU_ASSERT_EQUAL(msg.type, 2);

    CU_ASSERT_EQUAL(osal_mq_recv(mq, &msg, sizeof(msg_t), 1000), OSAL_API_OK);
    CU_ASSERT_EQUAL(msg.type, 3);

    CU_ASSERT_NOT_EQUAL(osal_mq_recv(mq, &msg, sizeof(msg_t), 0), OSAL_API_OK);
    CU_ASSERT_NOT_EQUAL(osal_mq_recv(mq, &msg, sizeof(msg_t), 1000), OSAL_API_OK);

    osal_task_destroy(task);

    osal_task_t task2;

    task2 = osal_task_create(routine2, (void *)mq, &task_attr);
    CU_ASSERT_PTR_NOT_NULL(task2);

    osal_task_sleep(1);

    osal_mq_destroy(mq);
    osal_task_destroy(task2);
}

void t_osal_mq_001(void)
{
    CU_add_test(mq_suite, "mq_01", mq_01);
}
