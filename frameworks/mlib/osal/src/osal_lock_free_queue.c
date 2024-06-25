#if !defined(no_use_lock_free_queue)

#include "osal_lock_free_queue.h"

#include "osal_api.h"

osal_lock_free_queue_t *osal_lock_free_queue_init(uint32_t msg_size, uint32_t msg_max)
{
    if (msg_size <= 0 || msg_max <= 0)
    {
        return NULL;
    }

    osal_lock_free_queue_t *queue = (osal_lock_free_queue_t *)osal_calloc(1, sizeof(osal_lock_free_queue_t) + (msg_size * msg_max));
    if (queue == NULL)
    {
        return NULL;
    }

    queue->read = 0;
    queue->write = 0;
    queue->msg_size = msg_size;
    queue->total_size = msg_size * msg_max;

    return queue;
}

int osal_lock_free_queue_is_full(osal_lock_free_queue_t *queue)
{
    if (queue == NULL)
    {
        return -1;
    }

    if ((queue->write + queue->msg_size) % queue->total_size == queue->read)
    {
        return 1;
    }

    return 0;
}

int osal_lock_free_queue_is_empty(osal_lock_free_queue_t *queue)
{
    if (queue == NULL)
    {
        return -1;
    }

    if (queue->read == queue->write)
    {
        return 1;
    }

    return 0;
}

int osal_lock_free_queue_push(osal_lock_free_queue_t *queue, const void *msg)
{
    if (queue == NULL || msg == NULL)
    {
        return -1;
    }

    if (osal_lock_free_queue_is_full(queue))
    {
        return -1;
    }

    uint32_t copy_len = (queue->total_size - queue->write < queue->msg_size) ? (queue->total_size - queue->write) : queue->msg_size;
    memcpy(&queue->buffer[queue->write], msg, copy_len);

    msg += copy_len;
    copy_len = queue->msg_size - copy_len;
    if (copy_len > 0)
    {
        memcpy(queue->buffer, msg, copy_len);
    }

    queue->write = (queue->write + queue->msg_size) % queue->total_size;

    return 0;
}

int osal_lock_free_queue_pop(osal_lock_free_queue_t *queue, void *msg, uint32_t msg_size)
{
    if (queue == NULL || msg == NULL)
    {
        return -1;
    }

    if (osal_lock_free_queue_is_empty(queue))
    {
        return -1;
    }

    uint32_t copy_len = (queue->total_size - queue->read < queue->msg_size) ? (queue->total_size - queue->read) : queue->msg_size;
    memcpy(msg, &queue->buffer[queue->read], copy_len);

    msg += copy_len;
    copy_len = queue->msg_size - copy_len;
    if (copy_len > 0)
    {
        memcpy(msg, queue->buffer, copy_len);
    }

    queue->read = (queue->read + queue->msg_size) % queue->total_size;

    return 0;
}

int osal_lock_free_queue_deinit(osal_lock_free_queue_t *queue)
{
    if (queue == NULL)
    {
        return -1;
    }

    osal_free(queue);

    return 0;
}

#endif // !no_use_lock_free_queue
