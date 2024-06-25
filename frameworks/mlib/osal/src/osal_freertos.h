#ifndef _OSAL_FREERTOS_H_
#define _OSAL_FREERTOS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stddef.h>

#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_random.h"
#include "esp_system.h"
#include "esp_err.h"

// translate freeRTOS error code to osal error code
#define FREERTOS_CALL(err) ((err) == pdPASS ? 0 : -1)

#ifndef osal_malloc
#define osal_malloc osal_api.malloc
#define osal_free free
#define osal_calloc osal_api.calloc
#define osal_realloc osal_api.realloc
#endif // !osal_malloc

#ifndef osal_get_version
#define osal_get_version() esp_get_idf_version()
#endif // !osal_get_version

#ifndef osal_task_create
#define osal_task_create osal_api.task_create
#define osal_task_self() xTaskGetCurrentTaskHandle()
#define osal_task_sleep(S) vTaskDelay(pdMS_TO_TICKS(S * 1000))
#define osal_task_usleep(US) usleep(US)
#define osal_task_destroy(xTask) vTaskDelete((TaskHandle_t)xTask)
#define osal_task_yield() taskYIELD()
#define osal_task_suspend(xTask) vTaskSuspend((TaskHandle_t)xTask)
#define osal_task_resume(xTask) vTaskResume((TaskHandle_t)xTask)
#define osal_task_get_priority(xTask) uxTaskPriorityGet(xTask)
#define osal_task_set_priority(xTask, uxNewPriority) vTaskPrioritySet(xTask, uxNewPriority)
#endif // !osal_task_create

#ifndef osal_mutex_create
#define osal_mutex_create() xSemaphoreCreateMutex()
#define osal_mutex_destroy(xMutex) vSemaphoreDelete(xMutex)
#define osal_mutex_lock(xMutex, MS) FREERTOS_CALL(xSemaphoreTake(xMutex, pdMS_TO_TICKS(MS)))
#define osal_mutex_trylock(xMutex) FREERTOS_CALL(xSemaphoreTake(xMutex, 0))
#define osal_mutex_unlock(xMutex) FREERTOS_CALL(xSemaphoreGive(xMutex))
#endif // !osal_mutex_create

#ifndef osal_sem_create
#define osal_sem_create(uxMaxCount, uxInitialCount) xSemaphoreCreateCounting((uxMaxCount), (uxInitialCount))
#define osal_sem_destroy(xSemaphore) vSemaphoreDelete(xSemaphore)
#define osal_sem_wait(xSemaphore, MS) FREERTOS_CALL(xSemaphoreTake(xSemaphore, pdMS_TO_TICKS(MS)))
#define osal_sem_post(xSemaphore, MS) FREERTOS_CALL(xSemaphoreGive(xSemaphore))
#endif // !osal_sem_create

#ifndef osal_mq_create
#define osal_mq_create(name, uxItemSize, uxQueueLength, flag) xQueueCreate((uxQueueLength), (uxItemSize))
#define osal_mq_destroy(xQueue) vQueueDelete(xQueue)
#define osal_mq_send(xQueue, pvItemToQueue, msg_size, MS) FREERTOS_CALL(xQueueSend(xQueue, pvItemToQueue, pdMS_TO_TICKS(MS)))
#define osal_mq_recv(xQueue, pvBuffer, msg_size, MS) FREERTOS_CALL(xQueueReceive(xQueue, pvBuffer, pdMS_TO_TICKS(MS)))
#endif // !osal_mq_create

#ifndef osal_random
#define osal_random() esp_random()
#endif // !osal_random

#endif // !_OSAL_FREERTOS_H_
