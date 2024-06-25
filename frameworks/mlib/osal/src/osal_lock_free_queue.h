#ifndef _OSAL_LOCK_FREE_QUEUE_H_
#define _OSAL_LOCK_FREE_QUEUE_H_

#if !defined(no_use_lock_free_queue)

#include <stdio.h>
#include <stdint.h>

typedef struct osal_lock_free_queue
{
    uint32_t write;
    uint32_t read;
    uint32_t msg_size;
    uint32_t total_size;
    uint8_t buffer[0];
} osal_lock_free_queue_t;

osal_lock_free_queue_t *osal_lock_free_queue_init(uint32_t msg_size, uint32_t msg_max);

int osal_lock_free_queue_is_full(osal_lock_free_queue_t *queue);

int osal_lock_free_queue_is_empty(osal_lock_free_queue_t *queue);

int osal_lock_free_queue_push(osal_lock_free_queue_t *queue, const void *msg);

int osal_lock_free_queue_pop(osal_lock_free_queue_t *queue, void *msg, uint32_t msg_size);

int osal_lock_free_queue_deinit(osal_lock_free_queue_t *queue);

#endif // !no_use_lock_free_queue

#endif
