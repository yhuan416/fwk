#include <stdio.h>
#include <stdlib.h>

#include "pthread.h"
#include <stdarg.h>

#include "adapter.h"

long mmem_lock(unsigned long lock)
{
    int ret = -1;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    switch (lock)
    {
    case MMEM_UNLOCK:
        ret = pthread_mutex_unlock(&mutex);
        break;
    case MMEM_LOCK:
        ret = pthread_mutex_lock(&mutex);
        break;
    default:
        ret = -1;
        break;
    }
    return ret;
}

#define MMEM_PRINT_BUF_SIZE (1024)

int mmem_printf(const int level, const char *fmt, ...)
{
    int ret;
    va_list args;
    char print_buf[MMEM_PRINT_BUF_SIZE] = {0};

    va_start(args, fmt);
    ret = vsnprintf(print_buf, MMEM_PRINT_BUF_SIZE, fmt, args);
    va_end(args);

    switch (level)
    {
    case MMEM_LEVEL_ERROR:
        printf("\n\x1B[31m[ERROR]\x1B[0m %s\n", print_buf);
        break;
    case MMEM_LEVEL_DEBUG:
        printf("\n\x1B[32m[DEBUG]\x1B[0m %s\n", print_buf);
        break;
    default:
        break;
    }

    return 0;
}
