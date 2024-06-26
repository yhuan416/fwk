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

#define OSAL_DEFAULT_PRIORITY ((UBaseType_t)15)
#define OSAL_DEFAULT_STACK_NAME "osal_task"
#define OSAL_DEFAULT_STACK_SIZE (configMINIMAL_STACK_SIZE)

osal_task_t osal_freertos_task_create(osal_task_func_t func,
                                      void *arg,
                                      const osal_task_attr_t *attr)
{
    BaseType_t ret;
    TaskHandle_t task_handle = NULL;
    struct task_wrapper_t *task_wrapper = NULL;
    uint32_t ulStackDepth = OSAL_DEFAULT_STACK_SIZE;
    UBaseType_t uxPriority = OSAL_DEFAULT_PRIORITY;
    const char *pcName = OSAL_DEFAULT_STACK_NAME;
    UBaseType_t core_aff = tskNO_AFFINITY;

    if (func == NULL)
    {
        ESP_LOGE(TAG, "osal_freertos_task_create: invalid parameters");
        return NULL;
    }

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    StackType_t *pxStackBuffer = NULL;
    StaticTask_t *pxTaskBuffer = NULL;
#endif

    if (attr != NULL)
    {
        if (attr->stack_size > configMINIMAL_STACK_SIZE)
        {
            ulStackDepth = attr->stack_size;
        }

        if (attr->priority >= configMAX_PRIORITIES)
        {
            ESP_LOGE(TAG, "osal_freertos_task_create: invalid priority");
            return NULL;
        }
        uxPriority = (UBaseType_t)attr->priority;

        if (attr->name != NULL)
        {
            pcName = attr->name;
        }

        if (attr->affinity_mask != 0)
        {
            core_aff = (UBaseType_t)attr->affinity_mask;
        }

#if (configSUPPORT_STATIC_ALLOCATION == 1)
        if ((attr->stack_start != NULL) &&
            (attr->task_cb_start != NULL) && (attr->task_cb_size >= sizeof(StaticTask_t)))
        {
            // stack size too small
            if (attr->stack_size < configMINIMAL_STACK_SIZE) {
                ESP_LOGE(TAG, "osal_freertos_task_create: invalid stack size");
                return NULL;
            }

            pxStackBuffer = attr->stack_start;
            pxTaskBuffer = attr->task_cb_start;
        }
#endif
    }

    task_wrapper = osal_malloc(sizeof(struct task_wrapper_t));
    if (task_wrapper == NULL)
    {
        ESP_LOGE(TAG, "osal_freertos_task_create: malloc failed");
        return NULL;
    }

    task_wrapper->func = func;
    task_wrapper->arg = arg;

#if (configSUPPORT_STATIC_ALLOCATION == 1)
    if (pxStackBuffer != NULL && pxTaskBuffer != NULL)
    {

#if ((configNUM_CORES > 1) && (configUSE_CORE_AFFINITY == 1))
        // freertos 增加的 xTaskCreateStaticAffinitySet 函数
        task_handle = xTaskCreateStaticAffinitySet(osal_freertos_task_wrapper, pcName, ulStackDepth, (void *)task_wrapper, uxPriority, pxStackBuffer, pxTaskBuffer, core_aff);
#else
        // esp-idf 原生的 xTaskCreateStaticPinnedToCore 函数
        task_handle = xTaskCreateStaticPinnedToCore(osal_freertos_task_wrapper, pcName, ulStackDepth, (void *)task_wrapper, uxPriority, pxStackBuffer, pxTaskBuffer, core_aff);
#endif

        if (task_handle == NULL)
        {
            ESP_LOGE(TAG, "osal_freertos_task_create: xTaskCreateStatic failed");
            osal_free(task_wrapper);
            return NULL;
        }

        return task_handle;
    }
#endif

#if ((configNUM_CORES > 1) && (configUSE_CORE_AFFINITY == 1))
    // freertos 增加的 xTaskCreateAffinitySet 函数
    ret = xTaskCreateAffinitySet(osal_freertos_task_wrapper, pcName, ulStackDepth, (void *)task_wrapper, uxPriority, core_aff, &task_handle);
#else
    // esp-idf 原生的 xTaskCreatePinnedToCore 函数
    ret = xTaskCreatePinnedToCore(osal_freertos_task_wrapper, pcName, ulStackDepth, (void *)task_wrapper, uxPriority, &task_handle, core_aff);
#endif
    if (ret != pdPASS)
    {
        ESP_LOGE(TAG, "osal_freertos_task_create: xTaskCreate failed");
        osal_free(task_wrapper);
        return NULL;
    }

    return task_handle;
}

int osal_freertos_task_join(osal_task_t task)
{
    ESP_LOGE(TAG, "osal_freertos_task_join: unsupported");
    return OSAL_API_FAIL;
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
    .task_join = osal_freertos_task_join,

    .reboot = osal_freertos_reboot,
};

#endif
