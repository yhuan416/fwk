#include <stdio.h>

#include "osal.h"

int flag = 0;

void *taskEntry(void *arg)
{
    int ret;
    printf("taskEntry\n");

    osal_sem_t sem = (osal_sem_t)arg;

    while (flag == 0)
    {
        ret = osal_sem_take(sem, 0);
        if (ret == OSAL_OK) {
            printf("taskEntry: osal_sem_take\n");
        } else {
            printf("taskEntry: osal_sem_take failed\n");
        }
    }

    printf("taskEntry: exit\n");

    return NULL;
}

int main(int argc, char const *argv[])
{
    osal_task_attr_t attr = {
        .name = "",
        .stackSize = 0,
        .affinity = 0,
        .priority = 0,
    };

    osal_sem_t sem = osal_sem_create(0, 1);

    osal_task_t task = osal_task_create(taskEntry, sem, &attr);

    osal_sem_give(sem, 0);
    osal_task_delay(1);

    osal_sem_give(sem, 0);
    osal_task_delay(1);

    osal_sem_give(sem, 0);
    osal_task_delay(1);

    flag = 1;// exit taskEntry

    osal_sem_give(sem, 0);
    osal_task_delay(1);

    // wait task exit
    osal_task_join(task);

    osal_sem_destroy(sem);

    return 0;
}
