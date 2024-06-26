/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015-2017, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: linux demo.
 * Created on: 2015-07-30
 */

#define LOG_TAG "main"

#include "elog.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <flashdb.h>

#define FDB_LOG_TAG "[main]"

static pthread_mutex_t kv_locker, ts_locker;
static uint32_t boot_count = 0;
static time_t boot_time[10] = {0, 1, 2, 3};
/* default KV nodes */
static struct fdb_default_kv_node default_kv_table[] = {
    {"username", "armink", 0},                       /* string KV */
    {"password", "123456", 0},                       /* string KV */
    {"boot_count", &boot_count, sizeof(boot_count)}, /* int type KV */
    {"boot_time", &boot_time, sizeof(boot_time)},    /* int array type KV */
};
/* KVDB object */
static struct fdb_kvdb kvdb = {0};
/* TSDB object */
struct fdb_tsdb tsdb = {0};
/* counts for simulated timestamp */
static int counts = 0;

extern void kvdb_basic_sample(fdb_kvdb_t kvdb);
extern void kvdb_type_string_sample(fdb_kvdb_t kvdb);
extern void kvdb_type_blob_sample(fdb_kvdb_t kvdb);
extern void tsdb_sample(fdb_tsdb_t tsdb);

static void lock(fdb_db_t db)
{
    pthread_mutex_lock((pthread_mutex_t *)db->user_data);
}

static void unlock(fdb_db_t db)
{
    pthread_mutex_unlock((pthread_mutex_t *)db->user_data);
}

static fdb_time_t get_time(void)
{
    return time(NULL);
}

static void test_elog(void);

int main(void)
{

    fdb_err_t result;
    bool file_mode = true;
    uint32_t sec_size = 4096, db_size = sec_size * 4;

#ifdef FDB_USING_KVDB
    { /* KVDB Sample */
        struct fdb_default_kv default_kv;

        default_kv.kvs = default_kv_table;
        default_kv.num = sizeof(default_kv_table) / sizeof(default_kv_table[0]);
        /* set the lock and unlock function if you want */
        pthread_mutex_init(&kv_locker, NULL);
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_LOCK, (void *)lock);
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_UNLOCK, (void *)unlock);
        /* set the sector and database max size */
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_SEC_SIZE, &sec_size);
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_MAX_SIZE, &db_size);
        /* enable file mode */
        fdb_kvdb_control(&kvdb, FDB_KVDB_CTRL_SET_FILE_MODE, &file_mode);
        /* create database directory */
        mkdir("fdb_kvdb1", 0777);
        /* Key-Value database initialization
         *
         *       &kvdb: database object
         *       "env": database name
         * "fdb_kvdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         * &default_kv: The default KV nodes. It will auto add to KVDB when first initialize successfully.
         *  &kv_locker: The locker object.
         */
        result = fdb_kvdb_init(&kvdb, "env", "fdb_kvdb1", &default_kv, &kv_locker);

        if (result != FDB_NO_ERR)
        {
            return -1;
        }

        /* run basic KV samples */
        kvdb_basic_sample(&kvdb);
        /* run string KV samples */
        kvdb_type_string_sample(&kvdb);
        /* run blob KV samples */
        kvdb_type_blob_sample(&kvdb);
    }
#endif /* FDB_USING_KVDB */

#ifdef FDB_USING_TSDB
    { /* TSDB Sample */
        /* set the lock and unlock function if you want */
        pthread_mutex_init(&ts_locker, NULL);
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_LOCK, (void *)lock);
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_UNLOCK, (void *)unlock);
        /* set the sector and database max size */
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_SEC_SIZE, &sec_size);
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_MAX_SIZE, &db_size);
        /* enable file mode */
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_SET_FILE_MODE, &file_mode);
        /* create database directory */
        mkdir("fdb_tsdb1", 0777);
        /* Time series database initialization
         *
         *       &tsdb: database object
         *       "log": database name
         * "fdb_tsdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         *    get_time: The get current timestamp function.
         *         128: maximum length of each log
         *   ts_locker: The locker object.
         */
        result = fdb_tsdb_init(&tsdb, "log", "fdb_tsdb1", get_time, 128, &ts_locker);
        /* read last saved time for simulated timestamp */
        fdb_tsdb_control(&tsdb, FDB_TSDB_CTRL_GET_LAST_TIME, &counts);

        if (result != FDB_NO_ERR)
        {
            return -1;
        }

        /* run TSDB sample */
        tsdb_sample(&tsdb);
    }
#endif /* FDB_USING_TSDB */

    /* close printf buffer */
    setbuf(stdout, NULL);
    /* initialize EasyLogger */
    elog_init();

    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
#ifdef ELOG_COLOR_ENABLE
    elog_set_text_color_enabled(true);
#endif
    /* start EasyLogger */
    elog_start();

    /* dynamic set enable or disable for output logs (true or false) */
    //    elog_set_output_enabled(false);
    /* dynamic set output logs's level (from ELOG_LVL_ASSERT to ELOG_LVL_VERBOSE) */
    //    elog_set_filter_lvl(ELOG_LVL_WARN);
    /* dynamic set output logs's filter for tag */
    //    elog_set_filter_tag("main");
    /* dynamic set output logs's filter for keyword */
    //    elog_set_filter_kw("Hello");
    /* dynamic set output logs's tag filter */
    //    elog_set_filter_tag_lvl("main", ELOG_LVL_WARN);

    /* test logger output */
    test_elog();

    return EXIT_SUCCESS;
}

/**
 * EasyLogger demo
 */
void test_elog(void)
{
    uint8_t buf[256] = {0};
    int i = 0;

    for (i = 0; i < sizeof(buf); i++)
    {
        buf[i] = i;
    }
    while (true)
    {
        /* test log output for all level */
        log_a("Hello EasyLogger!");
        log_e("Hello EasyLogger!");
        log_w("Hello EasyLogger!");
        log_i("Hello EasyLogger!");
        log_d("Hello EasyLogger!");
        log_v("Hello EasyLogger!");
        //        elog_raw("Hello EasyLogger!");
        elog_hexdump("test", 16, buf, sizeof(buf));
        sleep(5);
    }
}
