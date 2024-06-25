#include <elog.h>
#include <stdio.h>

#ifdef ELOG_FILE_ENABLE
#include <elog_file.h>
#endif

#include "osal_api.h"

static osal_mutex_t elog_mutex;

/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    elog_mutex = osal_mutex_create();
    if (elog_mutex == NULL) {
        return -1;
    }

#ifdef ELOG_FILE_ENABLE
    elog_file_init();
#endif

    return result;
}

/**
 * EasyLogger port deinitialize
 *
 */
void elog_port_deinit(void) {
#ifdef ELOG_FILE_ENABLE
    elog_file_deinit();
#endif

    osal_mutex_destroy(elog_mutex);
}


/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) {
    /* output to terminal */
#ifdef ELOG_TERMINAL_ENABLE
    printf("%.*s", (int)size, log);
#endif

#ifdef ELOG_FILE_ENABLE
    /* write the file */
    elog_file_write(log, size);
#endif 
}

/**
 * output lock
 */
void elog_port_output_lock(void) {
    osal_mutex_lock(elog_mutex, OSAL_API_WAITFOREVER);
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
    osal_mutex_unlock(elog_mutex);
}


/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {
    static char cur_system_time[24] = { 0 };

    time_t cur_t;
    struct tm cur_tm;

    time(&cur_t);
    localtime_r(&cur_t, &cur_tm);

    strftime(cur_system_time, sizeof(cur_system_time), "%Y-%m-%d %T", &cur_tm);

    return cur_system_time;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    static char cur_process_info[10] = { 0 };
    snprintf(cur_process_info, 10, "pid:%04d", osal_getpid());
    return cur_process_info;
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
    static char cur_thread_info[10] = { 0 };
    snprintf(cur_thread_info, 10, "tid:%04ld", osal_gettid());
    return cur_thread_info;
}
