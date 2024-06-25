#if defined(__linux__)

#include "osal_api.h"
#include "osal_posix.h"

#include <sys/prctl.h>

#include <errno.h>

#define mmodule_name "osal"

#if !defined(pr_fatal)

#if !defined(print_level)
#define print_level 2
#endif

#define pr_fatal(fmt, ...)          \
    if (print_level >= 1)           \
    {                               \
        printf(fmt, ##__VA_ARGS__); \
    }
#define pr_error(fmt, ...)          \
    if (print_level >= 2)           \
    {                               \
        printf(fmt, ##__VA_ARGS__); \
    }
#define pr_warn(fmt, ...)           \
    if (print_level >= 3)           \
    {                               \
        printf(fmt, ##__VA_ARGS__); \
    }
#define pr_info(fmt, ...)           \
    if (print_level >= 4)           \
    {                               \
        printf(fmt, ##__VA_ARGS__); \
    }
#define pr_debug(fmt, ...)          \
    if (print_level >= 5)           \
    {                               \
        printf(fmt, ##__VA_ARGS__); \
    }
#define pr_trace(fmt, ...)          \
    if (print_level >= 6)           \
    {                               \
        printf(fmt, ##__VA_ARGS__); \
    }
#endif

#define ALIGN_SIZE(size, align) (((size + align - 1) / align) * align)

static inline int errno_2_ret(int err)
{
    switch (err)
    {
    case ETIMEDOUT:
        return OSAL_API_TIMEDOUT;
        break;
    case EINVAL:
        return OSAL_API_INVAL;
        break;
    case EOVERFLOW:
        return OSAL_API_OVERFLOW;
        break;
    case ENOMEM:
        return OSAL_API_NOMEM;
        break;
    case EPERM:
        return OSAL_API_PERM;
        break;
    default:
        return OSAL_API_FAIL;
        break;
    }
}

void *osal_posix_malloc(size_t size)
{
    if (size == 0)
    {
        pr_warn("size is 0.\n");
        return NULL;
    }

    return malloc(size);
}

void *osal_posix_calloc(size_t num, size_t size)
{
    if (size == 0 || num == 0)
    {
        pr_warn("size or num is 0.\n");
        return NULL;
    }

    return calloc(num, size);
}

void *osal_posix_realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
    {
        pr_warn("ptr is NULL.\n");
        return osal_malloc(size);
    }

    if (size == 0)
    {
        pr_warn("size is 0.\n");
        osal_free(ptr);
        return NULL;
    }

    return realloc(ptr, size);
}

#define DEFAULT_STACK_NAME "osal_task"
#define OSAL_STACK_NAME_SIZE (16)

struct task_wrapper_t
{
    osal_task_func_t func;
    void *arg;
    char name[OSAL_STACK_NAME_SIZE];
};

static void *osal_task_wrapper(void *arg)
{
    struct task_wrapper_t *wrap = (struct task_wrapper_t *)arg;

    if (!wrap || !wrap->func)
    {
        pr_error("wrapper or wrapper->func is NULL.\n");
        return NULL;
    }

    osal_task_func_t func = wrap->func;
    void *args = wrap->arg;

    // set task name
    prctl(PR_SET_NAME, wrap->name, 0, 0, 0);

    // free wrap
    osal_free(wrap);
    wrap = NULL;

    // run task
    func(args);

    return NULL;
}

osal_task_t osal_posix_task_create(osal_task_func_t func,
                                   void *arg,
                                   const osal_task_attr_t *task_attr)
{
    int ret;
    pthread_t tid = (pthread_t)NULL;
    pthread_attr_t attr = {0};
    struct task_wrapper_t *wrap = NULL;
    uint32_t stack_size = 0;
    const char *name = NULL;

    if (func == NULL)
    {
        pr_error("func is NULL.\n");
        return NULL;
    }

    if (task_attr != NULL)
    {
        name = task_attr->name;
        stack_size = task_attr->stack_size;
    }

    if (ret = pthread_attr_init(&attr))
    {
        pr_error("pthread_attr_init fail, ret %d.\n", ret);
        return NULL;
    }

    if (ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED))
    {
        pr_error("pthread_attr_setdetachstate fail, ret %d.\n", ret);
        goto osal_posix_task_create_out;
    }

// align to STACK_SIZE_ALIGNED(4096)
#define STACK_SIZE_ALIGNED (4096)
    unsigned int stack_size_aligned = ALIGN_SIZE(stack_size, STACK_SIZE_ALIGNED);
    if (stack_size && (ret = pthread_attr_setstacksize(&attr, stack_size_aligned)))
    {
        pr_error("pthread_attr_setstacksize fail, stack_size_aligned %d, ret %d.\n", stack_size_aligned, ret);
        goto osal_posix_task_create_out;
    }

    // use default stack name
    if (name == NULL)
    {
        name = DEFAULT_STACK_NAME;
    }

    if (wrap = osal_calloc(1, sizeof(struct task_wrapper_t)))
    {
        const char *p;
        p = strrchr(name, '&'); // (xxx)&routine
        if (!p)
        {
            p = strrchr(name, ')'); // (xxx)routine
            if (!p)
            {
                p = name; // routine
            }
            else
            {
                p++;
            }
        }
        else
        {
            p++;
        }

        snprintf(wrap->name, sizeof(wrap->name), "%s", p);
        wrap->func = func;
        wrap->arg = arg;

        if (ret = pthread_create(&tid, &attr, osal_task_wrapper, (void *)wrap))
        {
            pr_error("pthread_create(wrap) failed, ret = %d.\n", ret);
            tid = (pthread_t)NULL;

            // free wrap
            free(wrap);
            wrap = NULL;

            goto osal_posix_task_create_out;
        }
    }
    else
    {
        if ((ret = pthread_create(&tid, &attr, func, arg)))
        {
            pr_error("pthread_create failed, ret = %d.\n", ret);
            tid = (pthread_t)NULL;
            goto osal_posix_task_create_out;
        }
    }

osal_posix_task_create_out:

    pthread_attr_destroy(&attr);
    return (osal_task_t)tid;
}

osal_mutex_t osal_posix_mutex_create(void)
{
    int ret;
    pthread_mutex_t *mutex = osal_malloc(sizeof(pthread_mutex_t));
    if (mutex == NULL)
    {
        pr_error("mutex malloc failed.\n");
        return NULL;
    }

    if (ret = pthread_mutex_init(mutex, NULL))
    {
        pr_error("pthread_mutex_init failed, ret = %d.\n", ret);
        osal_free(mutex);
        return NULL;
    }

    return (osal_mutex_t)mutex;
}

int osal_posix_mutex_destroy(osal_mutex_t mutex)
{
    if (mutex == NULL)
    {
        pr_error("mutex is NULL.\n");
        return OSAL_API_INVAL;
    }

    pthread_mutex_destroy((pthread_mutex_t *)mutex);
    osal_free(mutex);
    return OSAL_API_OK;
}

int osal_posix_mutex_lock(osal_mutex_t mutex, uint32_t timeout_ms)
{
    int ret;

    if (mutex == NULL)
    {
        pr_error("mutex is NULL.\n");
        return OSAL_API_INVAL;
    }

    if (ret = pthread_mutex_lock((pthread_mutex_t *)mutex))
    {
        pr_error("pthread_mutex_lock fail, ret = %d.\n", ret);
        return errno_2_ret(ret);
    }

    return OSAL_API_OK;
}

int osal_posix_mutex_trylock(osal_mutex_t mutex)
{
    int ret;

    if (mutex == NULL)
    {
        pr_error("mutex is NULL.\n");
        return OSAL_API_INVAL;
    }

    if (ret = pthread_mutex_trylock((pthread_mutex_t *)mutex))
    {
        pr_error("pthread_mutex_trylock fail, ret = %d.\n", ret);
        return errno_2_ret(ret);
    }

    return OSAL_API_OK;
}

int osal_posix_mutex_unlock(osal_mutex_t mutex)
{
    int ret;

    if (mutex == NULL)
    {
        pr_error("mutex is NULL.\n");
        return OSAL_API_INVAL;
    }

    if (ret = pthread_mutex_unlock((pthread_mutex_t *)mutex))
    {
        pr_error("pthread_mutex_unlock fail, ret = %d.\n", ret);
        return errno_2_ret(ret);
    }

    return OSAL_API_OK;
}

#include <semaphore.h>

osal_sem_t osal_posix_sem_create(uint32_t max, uint32_t init)
{
    int ret;
    sem_t *sem = (sem_t *)osal_calloc(1, sizeof(sem_t));

    if (sem == NULL)
    {
        pr_error("sem malloc failed.\n");
        return NULL;
    }

    if (ret = sem_init(sem, 0, init))
    {
        pr_error("sem_init failed, ret = %d.\n", ret);
        osal_free(sem);
        return NULL;
    }

    return (osal_sem_t)sem;
}

int osal_posix_sem_destroy(osal_sem_t sem)
{
    if (sem == NULL)
    {
        pr_error("sem is NULL.\n");
        return OSAL_API_INVAL;
    }

    sem_destroy((sem_t *)sem);
    osal_free(sem);
    return OSAL_API_OK;
}

int osal_posix_sem_wait(osal_sem_t sem, uint32_t timeout_ms)
{
    int ret;
    struct timespec tm = {0};

    if (sem == NULL)
    {
        pr_error("sem is NULL.\n");
        return OSAL_API_INVAL;
    }

    if (timeout_ms == 0)
    {
        ret = sem_trywait((sem_t *)sem);
    }
    else if (timeout_ms == OSAL_API_WAITFOREVER)
    {
        ret = sem_wait((sem_t *)sem);
    }
    else
    {
        osal_calc_timedwait(&tm, timeout_ms);
        ret = sem_timedwait((sem_t *)sem, &tm);
    }

    if (ret)
    {
        return errno_2_ret(errno);
    }

    return OSAL_API_OK;
}

int osal_posix_sem_post(osal_sem_t sem)
{
    if (sem == NULL)
    {
        pr_error("sem is NULL.\n");
        return OSAL_API_INVAL;
    }

    if (sem_post((sem_t *)sem))
    {
        return errno_2_ret(errno);
    }

    return OSAL_API_OK;
}

#if defined(no_use_lock_free_queue)

#include <fcntl.h>    /* For O_* constants */
#include <sys/stat.h> /* For mode constants */
#include <mqueue.h>

typedef struct osal_posix_mq_s
{
    mqd_t id;
    uint32_t msg_size;
    char name[0];
} osal_posix_mq_t;

#define str_start_with(str, c) ((str[0] == c) ? (1) : (0))

osal_mq_t osal_posix_mq_create(const char *name, long msg_size, long msg_max, int flag)
{
    mqd_t id;
    struct mq_attr attr = {0};

    if (name == NULL)
    {
        pr_error("osal_posix_mq_create, name is NULL.");
        return NULL;
    }

    osal_posix_mq_t *mq = (osal_posix_mq_t *)osal_calloc(1, sizeof(osal_posix_mq_t) + strlen(name) + 1);
    if (mq == NULL)
    {
        pr_error("osal_calloc failed.");
        return NULL;
    }

    if (str_start_with(name, '/'))
    {
        snprintf(mq->name, strlen(name), "%s", name);
    }
    else
    {
        snprintf(mq->name, strlen(name) + 1, "/%s", name);
    }

    // Set the queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = msg_max;
    attr.mq_msgsize = msg_size;
    attr.mq_curmsgs = 0;

    id = mq_open(mq->name, O_CREAT | O_RDWR | O_EXCL, 0644, &attr);
    if (id == -1)
    {
        pr_error("mq_open failed, ret : %d.", id);
        osal_free(mq);
        return NULL;
    }

    mq->id = id;
    mq->msg_size = msg_size;

    return mq;
}

int osal_posix_mq_destroy(osal_mq_t mq)
{
    if (mq == NULL)
    {
        pr_error("osal_posix_mq_destroy, mq is NULL.");
        return OSAL_API_INVAL;
    }

    osal_posix_mq_t *q = (osal_posix_mq_t *)mq;

    if (mq_close(q->id))
    {
        pr_error("mq_close, errno : %d.", errno);
    }

    if (mq_unlink(q->name))
    {
        pr_error("mq_unlink, errno : %d.", errno);
    }

    osal_free(q);

    return OSAL_API_OK;
}

int osal_posix_mq_send(osal_mq_t mq, const void *msg, int msg_size, int timeout_ms)
{
    int ret = 0;
    mqd_t id;
    struct timespec tm = {0};

    if (mq == NULL)
    {
        pr_error("osal_posix_mq_send, mq is NULL.");
        return OSAL_API_INVAL;
    }

    id = ((osal_posix_mq_t *)mq)->id;

    osal_calc_timedwait(&tm, timeout_ms);
    if (ret = mq_timedsend(id, msg, msg_size, 0, &tm))
    {
        return errno_2_ret(errno);
    }

    return OSAL_API_OK;
}

int osal_posix_mq_recv(osal_mq_t mq, void *msg, int msg_size, int timeout_ms)
{
    int ret = 0;
    mqd_t id;
    struct timespec tm = {0};

    if (mq == NULL)
    {
        pr_error("osal_posix_mq_send, mq is NULL.");
        return OSAL_API_INVAL;
    }

    id = ((osal_posix_mq_t *)mq)->id;

    osal_calc_timedwait(&tm, timeout_ms);
    if (mq_timedreceive(id, msg, msg_size, NULL, &tm) == -1)
    {
        return errno_2_ret(errno);
    }

    return OSAL_API_OK;
}

#else

#include "osal_lock_free_queue.h"

typedef struct osal_posix_mq_s
{
    int exit_flag;
    pthread_mutex_t w_mutex;
    pthread_mutex_t r_mutex;
    pthread_cond_t cond;
    osal_lock_free_queue_t *raw_queue;
} osal_posix_mq_t;

osal_mq_t osal_posix_mq_create(const char *name, long msg_size, long msg_max, int flag)
{
    osal_posix_mq_t *mq = NULL;
    int ret;

    if (msg_size <= 0 || msg_max <= 0)
    {
        pr_error("msg_size or msg_max is invalid.\n");
        return NULL;
    }

    mq = (osal_posix_mq_t *)osal_calloc(1, sizeof(struct osal_posix_mq_s));
    if (mq == NULL)
    {
        pr_error("mq malloc failed.\n");
        return NULL;
    }

    if (ret = pthread_mutex_init(&mq->w_mutex, NULL))
    {
        pr_error("pthread_mutex_init(w_mutex) failed, ret = %d.\n", ret);
        osal_free(mq);
        return NULL;
    }

    if (ret = pthread_mutex_init(&mq->r_mutex, NULL))
    {
        pr_error("pthread_mutex_init(r_mutex) failed, ret = %d.\n", ret);
        pthread_mutex_destroy(&mq->w_mutex);
        osal_free(mq);
        return NULL;
    }

    if (ret = pthread_cond_init(&mq->cond, NULL))
    {
        pr_error("pthread_cond_init failed, ret = %d.\n", ret);
        pthread_mutex_destroy(&mq->w_mutex);
        pthread_mutex_destroy(&mq->r_mutex);
        osal_free(mq);
    }

    mq->raw_queue = osal_lock_free_queue_init(msg_size, msg_max);
    if (mq->raw_queue == NULL)
    {
        pr_error("osal_lock_free_queue_init failed.\n");
        pthread_mutex_destroy(&mq->w_mutex);
        pthread_mutex_destroy(&mq->r_mutex);
        pthread_cond_destroy(&mq->cond);
        osal_free(mq);
    }

    return mq;
}

int osal_posix_mq_destroy(osal_mq_t mq)
{
    osal_posix_mq_t *mq_posix = (osal_posix_mq_t *)mq;

    if (mq_posix == NULL)
    {
        pr_error("mq is NULL.\n");
        return OSAL_API_INVAL;
    }

    mq_posix->exit_flag = 1;
    pthread_cond_broadcast(&mq_posix->cond);

    pthread_mutex_lock(&mq_posix->r_mutex);
    osal_lock_free_queue_deinit(mq_posix->raw_queue);
    pthread_mutex_unlock(&mq_posix->r_mutex);

    pthread_mutex_destroy(&mq_posix->w_mutex);
    pthread_mutex_destroy(&mq_posix->r_mutex);
    pthread_cond_destroy(&mq_posix->cond);

    osal_free(mq_posix);

    return OSAL_API_OK;
}

int osal_posix_mq_send(osal_mq_t mq, const void *msg, int msg_size, int timeout_ms)
{
    osal_posix_mq_t *mq_posix = (osal_posix_mq_t *)mq;
    int ret;

    if (mq_posix == NULL || msg == NULL || msg_size <= 0)
    {
        pr_error("mq or msg is NULL, or msg_size is invalid.\n");
        return OSAL_API_INVAL;
    }

    if (ret = pthread_mutex_lock(&mq_posix->w_mutex))
    {
        pr_error("pthread_mutex_lock(w_mutex) failed, ret = %d.\n", ret);
        return errno_2_ret(ret);
    }

    if (osal_lock_free_queue_push(mq_posix->raw_queue, msg))
    {
        pr_error("osal_lock_free_queue_push failed.\n");
        pthread_mutex_unlock(&mq_posix->w_mutex);
        return OSAL_API_FAIL;
    }

    if (ret = pthread_mutex_unlock(&mq_posix->w_mutex))
    {
        pr_error("pthread_mutex_unlock(w_mutex) failed, ret = %d.\n", ret);
        return errno_2_ret(ret);
    }

    if (ret = pthread_mutex_lock(&mq_posix->r_mutex))
    {
        pr_error("pthread_mutex_lock(r_mutex) failed, ret = %d.\n", ret);
        return errno_2_ret(ret);
    }

    if (ret = pthread_cond_signal(&mq_posix->cond))
    {
        pr_error("pthread_cond_signal failed, ret = %d.\n", ret);
        pthread_mutex_unlock(&mq_posix->r_mutex);
        return errno_2_ret(ret);
    }

    if (ret = pthread_mutex_unlock(&mq_posix->r_mutex))
    {
        pr_error("pthread_mutex_unlock(r_mutex) failed, ret = %d.\n", ret);
        return errno_2_ret(ret);
    }

    return OSAL_API_OK;
}

int osal_posix_mq_recv(osal_mq_t mq, void *msg, int msg_size, int timeout_ms)
{
    struct timespec tm = {0};
    osal_posix_mq_t *mq_posix = (osal_posix_mq_t *)mq;
    int ret;

    if (mq_posix == NULL || msg == NULL || msg_size <= 0)
    {
        pr_error("mq or msg is NULL, or msg_size is invalid.\n");
        return OSAL_API_INVAL;
    }

    if (ret = pthread_mutex_lock(&mq_posix->r_mutex))
    {
        pr_error("pthread_mutex_lock(r_mutex) failed, ret = %d.\n", ret);
        return errno_2_ret(ret);
    }

    while (osal_lock_free_queue_pop(mq_posix->raw_queue, msg, msg_size))
    {
        if (timeout_ms == 0)
        {
            ret = OSAL_API_TIMEDOUT;
            break;
        }

        if (timeout_ms == OSAL_API_WAITFOREVER)
        {
            ret = pthread_cond_wait(&mq_posix->cond, &mq_posix->r_mutex);
        }
        else
        {
            osal_calc_timedwait(&tm, timeout_ms);
            ret = pthread_cond_timedwait(&mq_posix->cond, &mq_posix->r_mutex, &tm);
        }

        if (ret == ETIMEDOUT)
        {
            ret = OSAL_API_TIMEDOUT;
            break;
        }
        else if (ret)
        {
            pr_error("pthread_cond_timedwait failed, ret = %d.\n", ret);
            ret = errno_2_ret(ret);
            break;
        }

        if (mq_posix->exit_flag)
        {
            ret = OSAL_API_FAIL;
            break;
        }
    }

    if (pthread_mutex_unlock(&mq_posix->r_mutex))
    {
        pr_error("pthread_mutex_unlock(r_mutex) failed.\n");
    }

    return ret;
}

#endif

int osal_posix_calc_timedwait(struct timespec *tm, uint32_t ms)
{
    int ret;
    struct timeval tv;

    if (tm == NULL)
    {
        pr_error("tm is NULL.\n");
        return OSAL_API_INVAL;
    }

    if (ret = gettimeofday(&tv, NULL))
    {
        pr_error("gettimeofday failed, ret = %d.\n", ret);
        return errno_2_ret(errno);
    }

    tv.tv_sec += ms / 1000;
    tv.tv_usec += (ms % 1000) * 1000;
    if (tv.tv_usec >= 1000000)
    {
        tv.tv_usec -= 1000000;
        tv.tv_sec++;
    }
    tm->tv_sec = tv.tv_sec;
    tm->tv_nsec = tv.tv_usec * 1000;

    return OSAL_API_OK;
}

#include <sys/sysinfo.h>

uint64_t osal_posix_uptime(void)
{
    struct sysinfo info;
    if (sysinfo(&info))
    {
        pr_error("sysinfo failed.\n");
        return 0;
    }
    return info.uptime;
}

osal_api_t osal_api = {
    .malloc = osal_posix_malloc,
    .calloc = osal_posix_calloc,
    .realloc = osal_posix_realloc,

    .task_create = osal_posix_task_create,

    .mutex_create = osal_posix_mutex_create,
    .mutex_destroy = osal_posix_mutex_destroy,
    .mutex_lock = osal_posix_mutex_lock,
    .mutex_trylock = osal_posix_mutex_trylock,
    .mutex_unlock = osal_posix_mutex_unlock,

    .sem_create = osal_posix_sem_create,
    .sem_destroy = osal_posix_sem_destroy,
    .sem_wait = osal_posix_sem_wait,
    .sem_post = osal_posix_sem_post,

    .mq_create = osal_posix_mq_create,
    .mq_destroy = osal_posix_mq_destroy,
    .mq_recv = osal_posix_mq_recv,
    .mq_send = osal_posix_mq_send,

    .uptime = osal_posix_uptime,
    .calc_timedwait = osal_posix_calc_timedwait,
};

#endif
