/**
 * @file osal_api.h
 * @author yhuan (yhuan416@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2024-03-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef _OSAL_API_H_
#define _OSAL_API_H_

#if defined(__linux__)
#include "osal_posix.h"
#elif defined(ESP_PLATFORM)
#include "osal_freertos.h"
#endif

enum osal_api_ret
{
    OSAL_API_OK = 0,   /*!< ok */
    OSAL_API_FAIL,     /*!< fail */
    OSAL_API_TIMEDOUT, /*!< time out */
    OSAL_API_INVAL,    /*!< invalid args */
    OSAL_API_NOMEM,    /*!< no mem */
    OSAL_API_OVERFLOW, /*!< overflow */
    OSAL_API_PERM,     /*!< perm */

    OSAL_API_RET_MAX,
};

#define OSAL_WAITFOREVER (-1)

//////////////////////////////////////////////// mem

/**
 * @brief malloc
 *
 * @name osal_malloc
 *
 * @param[in] size 内存大小
 *
 * @retval NULL 申请失败
 * @retval other 内存基地址
 */
typedef void *(*osal_api_malloc)(size_t size);

/**
 * @brief free
 *
 * @name osal_free
 *
 * @param[in] ptr 要释放的内存地址
 */
typedef void (*osal_api_free)(void *ptr);

/**
 * @brief calloc
 *
 * @name osal_calloc
 *
 * @param[in] num 数量
 * @param[in] size 单块内存大小
 *
 * @retval NULL 申请失败
 * @retval other 内存基地址
 */
typedef void *(*osal_api_calloc)(size_t num, size_t size);

/**
 * @brief realloc
 *
 * @name osal_realloc
 *
 * @param[in] ptr 内存基地址, 为 NULL 时, 该函数等同calloc
 * @param[in] size 内存大小, 为 0 时, 该函数等同free
 *
 * @retval 新申请到的地址
 */
typedef void *(*osal_api_realloc)(void *ptr, size_t size);

//////////////////////////////////////////////// task
typedef void *osal_task_t;
typedef void *(*osal_task_func_t)(void *arg);

typedef struct osal_task_attr_t
{
    const char *name;
    uint32_t stack_size;
    void *stack_start;
    uint32_t priority;
    uint32_t affinity_mask;
    uint32_t reserved;
} osal_task_attr_t;

/**
 * @brief 创建一个任务
 *
 * @name osal_task_create
 *
 * @param[in] func 任务入口
 * @param[in] arg 传递给任务的参数
 * @param[in] attr 任务属性
 *
 * @retval NULL 任务创建失败
 * @retval other 任务句柄
 */
typedef osal_task_t (*osal_api_task_create)(osal_task_func_t func,
                                            void *arg,
                                            const osal_task_attr_t *attr);

/**
 * @brief 等待某个任务结束
 *
 * @name osal_task_join
 *
 * @param[in] task 任务句柄
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_task_join)(osal_task_t task);

/**
 * @brief 销毁一个任务
 *
 * @name osal_task_destroy
 *
 * @param[in] task 任务句柄
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_task_destroy)(osal_task_t task);

/**
 * @brief 获取当前任务句柄
 *
 * @name osal_task_self
 *
 * @retval 任务句柄
 */
typedef osal_task_t (*osal_api_task_self)(void);

/**
 * @brief 任务调度, 一般用于rtos
 *
 * @name osal_task_tield
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_task_yield)(void);

/**
 * @brief 任务睡眠
 *
 * @name osal_task_sleep
 *
 * @param[in] s 秒数
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_task_sleep)(uint32_t s);

/**
 * @brief 任务微秒级睡眠
 *
 * @name osal_task_usleep
 *
 * @param[in] us 微秒秒数
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_task_usleep)(uint32_t us);

/**
 * @brief 任务挂起, 一般用于rtos
 *
 * @name osal_task_suspend
 *
 * @param[in] task 任务句柄
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_task_suspend)(osal_task_t task);

/**
 * @brief 任务恢复, 一般用于rtos
 *
 * @name osal_task_resume
 *
 * @param[in] task 任务句柄
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_task_resume)(osal_task_t task);

/**
 * @brief 获取任务优先级, 一般用于rtos
 *
 * @name osal_task_get_priority
 *
 * @param[in] task 任务句柄
 *
 * @retval >=0 任务优先级
 */
typedef int (*osal_api_task_get_priority)(osal_task_t task);

/**
 * @brief 设置任务优先级, 一般用于rtos
 *
 * @name osal_task_set_priority
 *
 * @param[in] task 任务句柄
 * @param[in] priority 任务优先级
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_task_set_priority)(osal_task_t task, int priority);

//////////////////////////////////////////////// mutex
typedef void *osal_mutex_t;

/**
 * @brief 创建一个互斥锁
 *
 * @name osal_mutex_create
 *
 * @retval NULL 失败
 * @retval other mutex句柄
 */
typedef osal_mutex_t (*osal_api_mutex_create)(void);

/**
 * @brief 销毁互斥锁
 *
 * @name osal_mutex_destroy
 *
 * @param[in] mutex mutex句柄
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_mutex_destroy)(osal_mutex_t mutex);

/**
 * @brief 持有互斥锁
 *
 * @name osal_mutex_lock
 *
 * @param[in] mutex mutex句柄
 * @param[in] ms 超时时间 ms
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_mutex_lock)(osal_mutex_t mutex, uint32_t ms);

/**
 * @brief 尝试持有互斥锁
 *
 * @name osal_mutex_trylock
 *
 * @param[in] mutex mutex句柄
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_mutex_trylock)(osal_mutex_t mutex);

/**
 * @brief 释放互斥锁
 *
 * @name osal_mutex_unlock
 *
 * @param[in] mutex mutex句柄
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_mutex_unlock)(osal_mutex_t mutex);

//////////////////////////////////////////////// sem
typedef void *osal_sem_t;

/**
 * @brief 创建一个信号量
 *
 * @name osal_sem_create
 *
 * @param[in] init 信号量初始值
 *
 * @retval NULL 创建失败
 * @retval other 信号量句柄
 */
typedef osal_sem_t (*osal_api_sem_create)(uint32_t max, uint32_t init);

/**
 * @brief 销毁信号量
 *
 * @name osal_sem_destroy
 *
 * @param[in] sem 信号量句柄
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_sem_destroy)(osal_sem_t sem);

/**
 * @brief 等待信号量
 *
 * @name osal_sem_wait
 *
 * @param[in] sem 信号量句柄
 * @param[in] ms 等待超时时间 ms
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_sem_wait)(osal_sem_t sem, uint32_t ms);

/**
 * @brief 释放信号量
 *
 * @name osal_sem_post
 *
 * @param[in] sem 信号量句柄
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_sem_post)(osal_sem_t sem);

////////////////////////////////////////////////// shm

/**
 * @brief 创建一个共享内存
 *
 * @name osal_shm_create
 *
 * @param[in] key key, 用于创建共享内存
 * @param[in] size 共享内存大小
 *
 * @retval NULL 创建失败
 * @retval other 共享内存地址
 */
typedef void *(*osal_api_shm_create)(const char *key, int size);

/**
 * @brief 销毁一个共享内存
 *
 * @name osal_shm_destroy
 *
 * @param[in] shm 共享内存地址
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_shm_destroy)(void *shm);

//////////////////////////////////////////////// event
typedef void *osal_event_t;

/**
 * @brief 创建一个事件
 *
 * @name osal_event_create
 *
 * @retval NULL 创建失败
 * @retval other 事件句柄
 */
typedef osal_event_t (*osal_api_event_create)(void);

/**
 * @brief 销毁事件
 *
 * @name osal_event_destroy
 *
 * @param[in] event 事件句柄
 */
typedef void (*osal_api_event_destroy)(osal_event_t event);

/**
 * @brief 等待对应的事件
 *
 * @name osal_event_wait
 *
 * @param[in] event 事件句柄
 * @param[in] set 等待的标记位
 * @param[in] option 选项 [ and | or | clear ]
 * @param[in] ms 超时时间 ms
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_event_wait)(osal_event_t event, uint32_t set, uint32_t option, uint32_t ms);

/**
 * @brief 触发对应的事件
 *
 * @name osal_event_set
 *
 * @param[in] event 事件句柄
 * @param[in] set 触发的标记位
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_event_set)(osal_event_t event, uint32_t set);

//////////////////////////////////////////////// mq
typedef void *osal_mq_t;

/**
 * @brief 创建消息队列
 *
 * @name osal_mq_create
 *
 * @param[in] name 消息队列名
 * @param[in] msg_size 消息大小
 * @param[in] msg_max 最大消息数
 * @param[in] flag 标志位
 *
 * @retval NULL 创建失败
 * @retval other 消息队列句柄
 */
typedef osal_mq_t (*osal_api_mq_create)(const char *name, long msg_size, long msg_max, int flag);

/**
 * @brief 销毁消息队列
 *
 * @name osal_mq_destroy
 *
 * @param[in] mq 消息队列句柄
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_mq_destroy)(osal_mq_t mq);

/**
 * @brief 发送消息
 *
 * @name osal_mq_send
 *
 * @param[in] mq 消息队列句柄
 * @param[in] msg 消息
 * @param[in] msg_size 消息大小
 * @param[in] ms 超时时间 ms
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_mq_send)(osal_mq_t mq, const void *msg, int msg_size, int ms);

/**
 * @brief 接受消息
 *
 * @name osal_mq_recv
 *
 * @param[in] mq 消息队列句柄
 * @param[out] msg 消息
 * @param[in] msg_size 消息大小
 * @param[in] ms 超时时间 ms
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_mq_recv)(osal_mq_t mq, void *msg, int msg_size, int ms);

//////////////////////////////////////////////// timer

//////////////////////////////////////////////// time

/**
 * @brief 计算超时时间
 *
 * @name osal_calc_timedwait
 *
 * @param[out] tm 返回的结构体
 * @param[in] ms 超时时间
 *
 * @retval OSAL_API_OK 成功
 * @retval other 失败
 */
typedef int (*osal_api_calc_timedwait)(struct timespec *tm, uint32_t ms);

//////////////////////////////////////////////// signal

//////////////////////////////////////////////// file

//////////////////////////////////////////////// misc
// system sysinfo env...

/**
 * @brief 设备重启
 *
 * @name osal_reboot
 */
typedef void (*osal_api_reboot)(void);

/**
 * @brief 获取osal版本
 *
 * @name osal_get_version
 *
 * @retval 版本号字符串
 */
typedef const char *(*osal_api_get_version)(void);

/**
 * @brief 获取设备启动时间
 *
 * @name osal_uptime
 *
 * @retval 启动时间
 */
typedef uint64_t (*osal_api_uptime)(void);

/**
 * @brief 获取一个随机数
 *
 * @name osal_random
 *
 * @retval 生成的随机数
 */
typedef uint32_t (*osal_api_random)(void);

/**
 * @brief get current process name
 * 
 * @retval pid
 */
typedef int32_t (*osal_api_getpid)(void);

/**
 * @brief get current thread name
 * 
 * @retval tid
 */
typedef long (*osal_api_gettid)(void);

typedef struct osal_api
{
    // mem
    osal_api_malloc malloc;
    osal_api_free free;
    osal_api_calloc calloc;
    osal_api_realloc realloc;

    // task
    osal_api_task_create task_create;
    osal_api_task_join task_join;
    osal_api_task_destroy task_destroy;
    osal_api_task_self task_self;
    osal_api_task_yield task_yield;
    osal_api_task_sleep task_sleep;
    osal_api_task_usleep task_usleep;
    osal_api_task_suspend task_suspend;
    osal_api_task_resume task_resume;
    osal_api_task_get_priority task_get_priority;
    osal_api_task_set_priority task_set_priority;

    // mutex
    osal_api_mutex_create mutex_create;
    osal_api_mutex_destroy mutex_destroy;
    osal_api_mutex_lock mutex_lock;
    osal_api_mutex_trylock mutex_trylock;
    osal_api_mutex_unlock mutex_unlock;

    // sem
    osal_api_sem_create sem_create;
    osal_api_sem_destroy sem_destroy;
    osal_api_sem_wait sem_wait;
    osal_api_sem_post sem_post;

    // shm
    osal_api_shm_create shm_create;
    osal_api_shm_destroy shm_destroy;

    // event
    osal_api_event_create event_create;
    osal_api_event_destroy event_destroy;
    osal_api_event_wait event_wait;
    osal_api_event_set event_set;

    // mq
    osal_api_mq_create mq_create;
    osal_api_mq_destroy mq_destroy;
    osal_api_mq_send mq_send;
    osal_api_mq_recv mq_recv;

    // time
    osal_api_calc_timedwait calc_timedwait;

    // misc
    osal_api_reboot reboot;
    osal_api_uptime uptime;
    osal_api_get_version get_version;
    osal_api_random random;
    osal_api_getpid getpid;
    osal_api_gettid gettid;
} osal_api_t;

extern osal_api_t osal_api;

#ifndef osal_malloc
#define osal_malloc osal_api.malloc
#define osal_free osal_api.free
#define osal_calloc osal_api.calloc
#define osal_realloc osal_api.realloc
#endif // !osal_malloc

#ifndef osal_task_create
#define osal_task_create osal_api.task_create
#define osal_task_join osal_api.task_join
#define osal_task_self osal_api.task_self
#define osal_task_sleep osal_api.task_sleep
#define osal_task_usleep osal_api.task_usleep
#define osal_task_destroy osal_api.task_destroy
#define osal_task_yield osal_api.task_yield
#define osal_task_suspend osal_api.task_suspend
#define osal_task_resume osal_api.task_resume
#define osal_task_get_priority osal_api.task_get_priority
#define osal_task_set_priority osal_api.task_set_priority
#endif // !osal_task_create

#ifndef osal_mutex_create
#define osal_mutex_create osal_api.mutex_create
#define osal_mutex_destroy osal_api.mutex_destroy
#define osal_mutex_lock osal_api.mutex_lock
#define osal_mutex_trylock osal_api.mutex_trylock
#define osal_mutex_unlock osal_api.mutex_unlock
#endif // !osal_mutex_create

#ifndef osal_sem_create
#define osal_sem_create osal_api.sem_create
#define osal_sem_destroy osal_api.sem_destroy
#define osal_sem_wait osal_api.sem_wait
#define osal_sem_post osal_api.sem_post
#endif // !osal_sem_create

#ifndef osal_shm_create
#define osal_shm_create osal_api.shm_create
#define osal_shm_destroy osal_api.shm_destroy
#endif // !osal_shm_create

#ifndef osal_event_create
#define osal_event_create osal_api.event_create
#define osal_event_destroy osal_api.event_destroy
#define osal_event_wait osal_api.event_wait
#define osal_event_set osal_api.event_set
#endif // !osal_event_create

#ifndef osal_mq_create
#define osal_mq_create osal_api.mq_create
#define osal_mq_destroy osal_api.mq_destroy
#define osal_mq_send osal_api.mq_send
#define osal_mq_recv osal_api.mq_recv
#endif // !osal_mq_create

#ifndef osal_uptime
#define osal_uptime osal_api.uptime
#endif // !osal_uptime

#ifndef osal_get_version
#define osal_get_version osal_api.get_version
#endif // !osal_get_version

#ifndef osal_calc_timedwait
#define osal_calc_timedwait osal_api.calc_timedwait
#endif // !osal_calc_timedwait

#ifndef osal_random
#define osal_random osal_api.random
#endif // !osal_random

#ifndef osal_reboot
#define osal_reboot osal_api.reboot
#endif // !osal_reboot

#ifndef osal_getpid
#define osal_getpid osal_api.getpid
#endif // !osal_getpid

#ifndef osal_gettid
#define osal_gettid osal_api.gettid
#endif // !osal_gettid

#endif // !_OSAL_API_H_
