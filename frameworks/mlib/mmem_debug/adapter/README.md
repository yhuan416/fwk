# mmem_debug_adapter

mmem_debug 适配层

## _real_malloc

``` c
// _real_malloc
#define _real_malloc(_size)                 malloc(_size)
#define _real_free(_addr)                   free(_addr)
#define _real_realloc(_addr, _size)         realloc(_addr, _size)
#define _real_calloc(_counts, _item_size)   calloc(_counts, _item_size)
```

定义mmem_debug库实际申请内存的接口.
由于mmem_debug库相当于接管了malloc/free/realloc/calloc的功能, 所以需要定义这些接口, 以便mmem_debug库在申请内存时, 调用这些接口.

## mmem_lock

``` c
// mmem_lock
#define MMEM_LOCK   (0x1)
#define MMEM_UNLOCK (0x2)

extern long mmem_lock(unsigned long lock);
```

mmem_debug库, 锁接口.
对接实际软件环境的线程锁相关接口.

## mmem_printf

``` c
// mmem_printf
#define MMEM_LEVEL_ERROR    (0x1)
#define MMEM_LEVEL_DEBUG    (0x2)

extern int mmem_printf(int level, const char *fmt, ...);

#define mmem_error(fmt, ...)   mmem_printf(MMEM_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define mmem_debug(fmt, ...)   mmem_printf(MMEM_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
```

对接打印接口, 用于打印mmem_debug库的调试信息.
源码中直接调用了 mmem_error/mmem_debug 接口, 请根据实际情况实现这两个接口.

***注意: 该接口调用的时候, mmem_debug库处于锁定状态, 不要在该接口运行的时候再次调用mmem_debug库的内存申请等接口, 会导致死锁.***


