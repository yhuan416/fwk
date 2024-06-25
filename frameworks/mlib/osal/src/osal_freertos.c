#if defined(ESP_PLATFORM)

#include "osal_api.h"
#include "osal_freertos.h"

#include "esp_log.h"

static const char *TAG = "osal";

void *osal_freertos_malloc(size_t size)
{
    if (size == 0)
    {
        ESP_LOGW(TAG, "osal_freertos_malloc: size is 0");
        return NULL;
    }

    return malloc(size);
}

void *osal_freertos_calloc(size_t num, size_t size)
{
    if (size == 0 || num == 0)
    {
        ESP_LOGW(TAG, "osal_freertos_calloc: size or num is 0");
        return NULL;
    }

    return calloc(num, size);
}

void *osal_freertos_realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
    {
        ESP_LOGW(TAG, "osal_freertos_realloc: ptr is NULL");
        return osal_malloc(size);
    }

    if (size == 0)
    {
        ESP_LOGW(TAG, "osal_freertos_realloc: size is 0");
        osal_free(ptr);
        return NULL;
    }

    return realloc(ptr, size);
}

struct task_wrapper_t
{
    osal_task_func_t func;
    void *arg;
};

static void osal_freertos_task_wrapper(void *arg)
{
    struct task_wrapper_t *task_wrapper = (struct task_wrapper_t *)arg;

    osal_task_func_t func = task_wrapper->func;
    void *func_arg = task_wrapper->arg;

    osal_free(task_wrapper);

    func(func_arg);

    vTaskDelete(NULL);
}

osal_task_t osal_freertos_task_create(const char *name,
                                      osal_task_func_t func,
                                      void *arg,
                                      void *stack_start,
                                      unsigned int stack_size,
                                      unsigned int priority)
{
    TaskHandle_t task_handle = NULL;
    struct task_wrapper_t *task_wrapper = NULL;

    if (stack_size <= 0 || priority >= configMAX_PRIORITIES || func == NULL || name == NULL)
    {
        ESP_LOGE(TAG, "osal_freertos_task_create: invalid parameters");
        return NULL;
    }

    task_wrapper = osal_malloc(sizeof(struct task_wrapper_t));
    if (task_wrapper == NULL)
    {
        ESP_LOGE(TAG, "osal_freertos_task_create: malloc failed");
        return NULL;
    }

    task_wrapper->func = func;
    task_wrapper->arg = arg;

    BaseType_t ret = xTaskCreate(osal_freertos_task_wrapper, name, stack_size, (void *)task_wrapper, priority, &task_handle);
    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "osal_freertos_task_create: xTaskCreate failed");
        osal_free(task_wrapper);
        return NULL;
    }

    return task_handle;
}

osal_task_t osal_freertos_task_create_pin_to_core(const char *name,
                                                  osal_task_func_t func,
                                                  void *arg,
                                                  void *stack_start,
                                                  unsigned int stack_size,
                                                  unsigned int priority,
                                                  int core_id)
{
    TaskHandle_t task_handle = NULL;
    struct task_wrapper_t *task_wrapper = NULL;

    if (stack_size <= 0 || priority >= configMAX_PRIORITIES || func == NULL || name == NULL)
    {
        ESP_LOGE(TAG, "osal_freertos_task_create_pin_to_core: invalid parameters");
        return NULL;
    }

    task_wrapper = osal_malloc(sizeof(struct task_wrapper_t));
    if (task_wrapper == NULL)
    {
        ESP_LOGE(TAG, "osal_freertos_task_create: malloc failed");
        return NULL;
    }

    task_wrapper->func = func;
    task_wrapper->arg = arg;

    BaseType_t ret = xTaskCreatePinnedToCore(osal_freertos_task_wrapper, name, stack_size, (void *)task_wrapper, priority, &task_handle, core_id);
    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "osal_freertos_task_create_pin_to_core: xTaskCreatePinnedToCore failed");
        osal_free(task_wrapper);
        return NULL;
    }

    return task_handle;
}

void osal_freertos_reboot(void)
{
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}

osal_api_t osal_api = {
    .malloc = osal_freertos_malloc,
    .calloc = osal_freertos_calloc,
    .realloc = osal_freertos_realloc,

    .task_create = osal_freertos_task_create,
    .task_create_pin_to_core = osal_freertos_task_create_pin_to_core,

    .reboot = osal_freertos_reboot,
};

#endif
