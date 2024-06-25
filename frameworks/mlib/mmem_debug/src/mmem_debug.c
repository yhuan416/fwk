#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "mlist.h"

#include "mmem_dump.h"

#include "adapter.h"

#ifndef _real_malloc
#error "_real_malloc undefined."
#endif // !_real_malloc

#ifndef _real_free
#error "_real_free undefined."
#endif // !_real_free

#ifndef _real_realloc
#error "_real_realloc undefined."
#endif // !_real_realloc

#ifndef _real_calloc
#error "_real_calloc undefined."
#endif // !_real_calloc

#define _mmem_crash()    \
    do                   \
    {                    \
        *((int *)0) = 0; \
    } while (0)

static void _mmem_lock(void)
{
    if (mmem_lock(MMEM_LOCK))
    {
        mmem_error("_mmem_lock(), failed.");
        _mmem_crash();
    }
}

static void _mmem_unlock(void)
{
    if (mmem_lock(MMEM_UNLOCK))
    {
        mmem_error("_mmem_unlock(), failed.");
        _mmem_crash();
    }
}

#define _align(_size, _align) (((_size) + ((_align)-1)) & (~((_align)-1)))

#define _mmem_align(_size) _align(_size, sizeof(long))

typedef struct mmem_block
{
    unsigned long magic;      // 头部魔数
    mlist list;               // 链表节点
    unsigned long size;       // 数据域大小
    unsigned long total_size; // 总大小
    const char *file;         // 文件名
    unsigned long line;       // 行号
    char data[0];             // 数据域变长数组
} mmem_block_t;

#define MMEM_BLOCK_SIZE _mmem_align(sizeof(mmem_block_t))
#define _mmem_total_size(_size) (unsigned long)(MMEM_BLOCK_SIZE + (_size) + sizeof(long))

#define _mmem_get_block(_addr) ((mmem_block_t *)((char *)(_addr)-MMEM_BLOCK_SIZE))

#define _mmem_block_magic(_block) ((_block)->magic)
#define _mmem_block_size(_block) ((_block)->size)
#define _mmem_block_total_size(_block) ((_block)->total_size)
#define _mmem_block_file(_block) ((_block)->file)
#define _mmem_block_line(_block) ((_block)->line)
#define _mmem_block_data(_block) (void *)(&((_block)->data))
#define _mmem_block_tail_magic(_block) (*((long *)((char *)((_block)->data) + (_block)->size)))

#define _mmem_block_update(_block, _size, _total_size, _file, _line) \
    do                                                               \
    {                                                                \
        _mmem_block_size(_block) = (_size);                          \
        _mmem_block_total_size(_block) = (_total_size);              \
        _mmem_block_file(_block) = (_file);                          \
        _mmem_block_line(_block) = (_line);                          \
    } while (0)

#define MMEM_BLOCK_ACTIVE_HEAD_MAGIC (*((long *)"mbah    "))
#define MMEM_BLOCK_ACTIVE_TAIL_MAGIC (*((long *)"mbat    "))
#define MMEM_BLOCK_FREE_HEAD_MAGIC (*((long *)"mbfh    "))
#define MMEM_BLOCK_FREE_TAIL_MAGIC (*((long *)"mbft    "))

#define _mmem_check_block_magic_active(_block)                        \
    ((_mmem_block_magic(_block) != MMEM_BLOCK_ACTIVE_HEAD_MAGIC ||    \
      _mmem_block_tail_magic(_block) != MMEM_BLOCK_ACTIVE_TAIL_MAGIC) \
         ? -1                                                         \
         : 0)

#define _mmem_set_block_magic_active(_block)                           \
    do                                                                 \
    {                                                                  \
        _mmem_block_magic(_block) = MMEM_BLOCK_ACTIVE_HEAD_MAGIC;      \
        _mmem_block_tail_magic(_block) = MMEM_BLOCK_ACTIVE_TAIL_MAGIC; \
    } while (0)

#define _mmem_set_block_magic_free(_block)                           \
    do                                                               \
    {                                                                \
        _mmem_block_magic(_block) = MMEM_BLOCK_FREE_HEAD_MAGIC;      \
        _mmem_block_tail_magic(_block) = MMEM_BLOCK_FREE_TAIL_MAGIC; \
    } while (0)

typedef struct mmem_block_table
{
    unsigned long count;
    mlist list;
    unsigned long total_size;
    unsigned long active_size;
    unsigned long max_total_size;
    unsigned long max_active_size;
} mmem_block_table_t;

static mmem_block_table_t mmem_block_table = {0};

static mmem_block_table_t *_mmem_block_table_get(void)
{
    static mmem_block_table_t *table = NULL;

    if (table == NULL)
    {
        table = &mmem_block_table;

        table->total_size = 0;
        table->active_size = 0;

        // init list head
        mlist_init(&(table->list));
    }

    return table;
}

#define _mmem_block_add(_table, _block)                        \
    do                                                         \
    {                                                          \
        mlist_add_tail(&((_table)->list), &((_block)->list));  \
        (_table)->count++;                                     \
        (_table)->total_size += (_block)->total_size;          \
        (_table)->active_size += (_block)->size;               \
        if ((_table)->active_size > (_table)->max_active_size) \
        {                                                      \
            (_table)->max_active_size = (_table)->active_size; \
        }                                                      \
        if ((_table)->total_size > (_table)->max_total_size)   \
        {                                                      \
            (_table)->max_total_size = (_table)->total_size;   \
        }                                                      \
    } while (0)

#define _mmem_block_del(_table, _block)               \
    do                                                \
    {                                                 \
        mlist_del(&((_block)->list));                 \
        (_table)->count--;                            \
        (_table)->total_size -= (_block)->total_size; \
        (_table)->active_size -= (_block)->size;      \
    } while (0)

void *mmem_calloc(unsigned long counts, unsigned long item_size, const char *file, int line)
{
    mmem_block_t *block = NULL;
    mmem_block_table_t *table = NULL;
    unsigned long size = 0;
    unsigned long total_size = 0;

    _mmem_lock();

    mmem_debug("mmem_calloc(%lu,%lu): enter.", counts, item_size);

    if (counts == 0 || item_size == 0)
    {
        mmem_error("mmem_calloc: invalid counts(%lu) or item_size(%lu)!", counts, item_size);
        _mmem_unlock();
        return NULL;
    }

    size = counts * item_size;
    total_size = _mmem_total_size(size);

    block = (mmem_block_t *)_real_calloc(1, total_size);
    if (block == NULL)
    {
        mmem_error("mmem_calloc: calloc failed!");
        _mmem_unlock();
        return NULL;
    }

    _mmem_block_update(block, size, total_size, file, line);

    _mmem_set_block_magic_active(block);

    // add to block table
    table = _mmem_block_table_get();
    mlist_init(&(block->list));
    _mmem_block_add(table, block);

    mmem_debug("mmem_calloc: exit.");

    _mmem_unlock();

    return _mmem_block_data(block);
}

void *mmem_alloc(unsigned long size, const char *file, int line)
{
    mmem_block_t *block = NULL;
    mmem_block_table_t *table = NULL;
    unsigned long total_size = 0;

    _mmem_lock();

    mmem_debug("mmem_alloc(%lu): enter.", size);

    if (size == 0)
    {
        mmem_error("mmem_alloc: invalid size(%lu)!", size);
        _mmem_unlock();
        return NULL;
    }

    total_size = _mmem_total_size(size);

    block = (mmem_block_t *)_real_malloc(total_size);
    if (block == NULL)
    {
        mmem_error("mmem_alloc: malloc failed!");
        _mmem_unlock();
        return NULL;
    }

    // update block info
    _mmem_block_update(block, size, total_size, file, line);

    // set block magic
    _mmem_set_block_magic_active(block);

    // add to block table
    table = _mmem_block_table_get();
    mlist_init(&(block->list));
    _mmem_block_add(table, block);

    mmem_debug("mmem_alloc: exit.");

    _mmem_unlock();

    return _mmem_block_data(block);
}

void mmem_free(void *addr, const char *file, int line)
{
    mmem_block_t *block = NULL;
    mmem_block_table_t *table = NULL;

    _mmem_lock();

    mmem_debug("mmem_free(%p): enter.", addr);

    if (addr == NULL)
    {
        mmem_error("mmem_free: invalid addr(%p)!", addr);
        _mmem_unlock();
        return;
    }

    block = _mmem_get_block(addr);
    if (_mmem_check_block_magic_active(block))
    {
        mmem_error("mmem_free: block(%p) magic error!, %s[%d] size:%d", block,
                   _mmem_block_file(block),
                   _mmem_block_line(block),
                   _mmem_block_size(block));
        _mmem_unlock();
        return;
    }

    // set block magic
    _mmem_set_block_magic_free(block);

    // delete old block
    table = _mmem_block_table_get();
    _mmem_block_del(table, block);

    _real_free(block);

    mmem_debug("mmem_free: exit.");

    _mmem_unlock();
}

void *mmem_realloc(void *addr, unsigned long size, const char *file, int line)
{
    mmem_block_t *block = NULL;
    mmem_block_t *new_block = NULL;
    mmem_block_table_t *table = NULL;
    unsigned long total_size = 0;

    _mmem_lock();

    mmem_debug("mmem_realloc(%p,%lu): enter.", addr, size);

    // if addr is NULL, realloc is equal to malloc
    if (addr == NULL)
    {
        _mmem_unlock();
        return mmem_alloc(size, file, line);
    }

    // if size is 0, realloc is equal to free
    if (size == 0)
    {
        _mmem_unlock();
        mmem_free(addr, file, line);
        return NULL;
    }

    block = _mmem_get_block(addr);
    if (_mmem_check_block_magic_active(block))
    {
        mmem_error("mmem_realloc: block(%p) magic error! %s[%d] size:%d", block,
                   _mmem_block_file(block),
                   _mmem_block_line(block),
                   _mmem_block_size(block));
        _mmem_unlock();
        return NULL;
    }

    // set old block magic to free
    _mmem_set_block_magic_free(block);

    // delete old block
    table = _mmem_block_table_get();
    _mmem_block_del(table, block);

    // alloc new block
    total_size = _mmem_total_size(size);
    new_block = (mmem_block_t *)_real_realloc(block, total_size);
    if (new_block == NULL)
    { // realloc failed, restore old block.
        // set old block magic to active
        _mmem_set_block_magic_active(block);

        // add old block to block table
        mlist_init(&(block->list));
        _mmem_block_add(table, block);

        mmem_error("mmem_realloc: realloc failed!");

        _mmem_unlock();

        return NULL;
    }

    block = new_block;

    // update new block
    _mmem_block_update(block, size, total_size, file, line);

    // set new block magic
    _mmem_set_block_magic_active(block);

    // add to block table
    mlist_init(&(block->list));
    _mmem_block_add(table, block);

    mmem_debug("mmem_realloc: exit.");

    _mmem_unlock();

    return _mmem_block_data(block);
}

// mmem_dump
static long _mmem_dump_cmd_counts(mmem_block_table_t *table, char *buf, unsigned long buf_size);
static long _mmem_dump_cmd_mmem_info(mmem_block_table_t *table, char *buf, unsigned long buf_size);
static long _mmem_dump_cmd_mmem_block_info(mmem_block_table_t *table, unsigned long counts, char *buf, unsigned long buf_size);

long mmem_dump(unsigned long cmd, unsigned long counts, void *buf, unsigned long buf_size)
{
    long ret = 0;
    mmem_block_table_t *table = NULL;

    _mmem_lock();

    mmem_debug("mmem_dump: enter.");

    table = _mmem_block_table_get();

    switch (cmd)
    {
    case MMEM_DUMP_CMD_COUNTS:
        ret = _mmem_dump_cmd_counts(table, buf, buf_size);
        break;

    case MMEM_DUMP_CMD_MMEM_INFO:
        ret = _mmem_dump_cmd_mmem_info(table, buf, buf_size);
        break;

    case MMEM_DUMP_CMD_MMEM_BLOCK_INFO:
        ret = _mmem_dump_cmd_mmem_block_info(table, counts, buf, buf_size);
        break;

    default:
        mmem_error("mmem_dump: invalid cmd %ld", cmd);
        ret = MMEM_DUMP_RET_INVALID_CMD;
        break;
    }

    mmem_debug("mmem_dump: exit.");

    _mmem_unlock();

    return ret;
}

// mmem_free_all
void mmem_free_all(void)
{
    mmem_block_t *block = NULL, *n = NULL;
    mmem_block_table_t *table = NULL;

    _mmem_lock();

    mmem_debug("mmem_free_all: enter.");

    table = _mmem_block_table_get();

    mlist_for_each_entry_safe(block, n, mmem_block_t, &(table->list), list)
    {

        // check block magic
        if (_mmem_check_block_magic_active(block))
        {
            mmem_error("mmem_free_all: block(%p) magic error! %s[%d] size:%d", block,
                       _mmem_block_file(block),
                       _mmem_block_line(block),
                       _mmem_block_size(block));
        }

        // set block magic
        _mmem_set_block_magic_free(block);

        // delete old block
        _mmem_block_del(table, block);

        // free block
        _real_free(block);
    }

    mmem_debug("mmem_free_all: exit.");

    _mmem_unlock();
}

// mmem_dump cmd
static long _mmem_dump_cmd_counts(mmem_block_table_t *table, char *buf, unsigned long buf_size)
{
    long ret = MMEM_DUMP_RET_OK;

    if (buf == NULL)
    {
        return MMEM_DUMP_RET_EMPTY_BUF;
    }

    if (buf_size < sizeof(table->count))
    {
        return MMEM_DUMP_RET_BUF_SIZE_TOO_SMALL;
    }

    *((unsigned long *)buf) = table->count;

    return ret;
}

static long _mmem_dump_cmd_mmem_info(mmem_block_table_t *table, char *buf, unsigned long buf_size)
{
    long ret = MMEM_DUMP_RET_OK;

    if (buf == NULL)
    {
        return MMEM_DUMP_RET_EMPTY_BUF;
    }

    if (buf_size < sizeof(mmem_info_t))
    {
        return MMEM_DUMP_RET_BUF_SIZE_TOO_SMALL;
    }

    mmem_info_t *info = (mmem_info_t *)buf;

    info->counts = table->count;
    info->total_size = table->total_size;
    info->active_size = table->active_size;
    info->max_total_size = table->max_total_size;
    info->max_active_size = table->max_active_size;

    return ret;
}

static long _mmem_dump_cmd_mmem_block_info(mmem_block_table_t *table, unsigned long counts, char *buf, unsigned long buf_size)
{
    long index = 0;
    long ret = MMEM_DUMP_RET_OK;
    mmem_block_t *block = NULL;
    unsigned long size = buf_size;
    mmem_block_info_t *info = (mmem_block_info_t *)buf;

    // check buf
    if (buf == NULL)
    {
        return MMEM_DUMP_RET_EMPTY_BUF;
    }

    // check buf size
    if (buf_size < sizeof(mmem_block_info_t))
    {
        return MMEM_DUMP_RET_BUF_SIZE_TOO_SMALL;
    }

    if (counts == 0)
    {
        return 0;
    }

    mlist_for_each_entry(block, mmem_block_t, &(table->list), list)
    {

        info[index].size = _mmem_block_size(block);
        info[index].total_size = _mmem_block_total_size(block);
        info[index].file = _mmem_block_file(block);
        info[index].line = _mmem_block_line(block);
        index++;

        // check left counts
        if (index >= counts)
        {
            break;
        }

        // check left buf size
        size -= sizeof(mmem_block_info_t);
        if (size < sizeof(mmem_block_info_t))
        {
            break;
        }
    }

    return index;
}
