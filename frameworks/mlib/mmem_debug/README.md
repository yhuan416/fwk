# (Monitor) Memory Debug Library

[![GitHub](https://img.shields.io/github/license/yhuan416/mmem_debug)](https://github.com/yhuan416/mmem_debug)
[![GitHub last commit (by committer)](https://img.shields.io/github/last-commit/yhuan416/mmem_debug)](https://github.com/yhuan416/mmem_debug)
[![GitHub Workflow Status (with event)](https://img.shields.io/github/actions/workflow/status/yhuan416/mmem_debug/every_day_build.yml)](https://github.com/yhuan416/mmem_debug/actions/workflows/every_day_build.yml)
<!--
![GitHub forks](https://img.shields.io/github/forks/yhuan416/mmem_debug)
![GitHub Repo stars](https://img.shields.io/github/stars/yhuan416/mmem_debug)
-->

内存调试工具，用于监控程序运行过程中的内存泄漏，内存越界等问题。

此库设计的初衷是为了能够方便的对程序的内存使用情况进行测试, 同时, 在正式发布版本中, 可以关闭相关功能, 减少内存占用以及性能损耗。

相关接口设计为非注册式。

在debug版本中, 打开 __mmem_debug_enbale 宏, 可以开启内存检测功能。  
此时, 程序中的所有 malloc 相关函数都被替换为 mmem_debug 中的接口, 以便进行内存检测。


在release版本中, 可以关闭 __mmem_debug_enbale 宏, 可以关闭内存检测功能。

## 1. 使用方法

在源文件中引入头文件 `mmem_debug.h`
``` c
#include "mmem_debug.h"
```

添加编译参数 -D__mmem_debug_enbale=1

**正常使用 malloc, calloc, realloc, free 等函数即可**

当不需要检测时，可以将编译参数 __mmem_debug_enbale 去掉或者设置为 0

## 2. 接口说明
*注：打开了 __mmem_debug_enbale 宏之后正常使用 malloc 等接口即可.*

``` c

/**
 * @brief mmem_calloc       calloc
 * @param counts[in]        item counts
 * @param item_size[in]     item size
 * @param file[in]          file name
 * @param line[in]          line number
 * @return (void *)         address
*/
extern void *mmem_calloc(unsigned long counts, unsigned long item_size, const char *file, int line);

/**
 * @brief mmem_alloc        malloc
 * @param size[in]          size to malloc
 * @param file[in]          file name
 * @param line[in]          line number
 * @return (void *)         address
*/
extern void *mmem_alloc(unsigned long size, const char *file, int line);

/**
 * @brief mmem_free         free
 * @param addr[in]          address to free
 * @param file[in]          file name
 * @param line[in]          line number
 * @return void
*/
extern void mmem_free(void *addr, const char *file, int line);

/**
 * @brief mmem_realloc      realloc
 * @param addr[in]          address to realloc, if NULL, it will be malloc
 * @param size[in]          size to realloc, if 0, it will be free
 * @param file[in]          file name
 * @param line[in]          line number
 * @return (void *)
*/
extern void *mmem_realloc(void *addr, unsigned long size, const char *file, int line);

/**
 * @brief mmem_free_all     free all memory
 * @return void
*/
extern void mmem_free_all(void);
```

## 3. dump内存信息

``` c
// dump command
#define MMEM_DUMP_CMD_COUNTS            (0x01)
#define MMEM_DUMP_CMD_MMEM_INFO         (0x02)
#define MMEM_DUMP_CMD_MMEM_BLOCK_INFO   (0x03)

/**
 * @brief mmem_dump         dump memory info
 * @param cmd[in]           command, see MMEM_DUMP_CMD_XXX
 * @param counts[in]        counts of data
 * @param buf[out]          buffer to store data
 * @param buf_size[in]      buffer size
 * @return (long)           >=0: success, <0: fail
*/
extern long mmem_dump(unsigned long cmd, unsigned long counts, void *buf, unsigned long buf_size);

...

// example
long ret = 0;
unsigned long counts = 0;
ret = mmem_dump(MMEM_DUMP_CMD_COUNTS, 0, (void *)&counts, sizeof(unsigned long));
if (ret == MMEM_DUMP_RET_OK) {
    printf("counts: %lu\n", counts);
}

mmem_info_t mmem_info = {0};
ret = mmem_dump(MMEM_DUMP_CMD_MMEM_INFO, 0, (void *)&mmem_info, sizeof(mmem_info_t));

mmem_block_info_t mmem_block_info[10] = {0};
ret = mmem_dump(MMEM_DUMP_CMD_MMEM_BLOCK_INFO, 10, (void *)mmem_block_info, sizeof(mmem_block_info_t) * 10);
if (ret > 0) {
    // dump success, ret is the counts of data
}

...
```

## 4. TODO

- [ ] 调整库整体接口, 支持封装层, 方便移植
    - [x] 临界区(锁)
    - [ ] 打印接口
- [ ] 统一模块内用到的打印接口, 方便对接日志模块
- [ ] 支持内存越界检测接口
