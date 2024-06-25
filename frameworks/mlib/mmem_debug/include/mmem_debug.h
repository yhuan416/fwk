#ifndef __MMEM_DEBUG_H__
#define __MMEM_DEBUG_H__

#include <stdlib.h>

#if defined(__cplusplus)
extern "C" {
#endif/* defined(__cplusplus) */

#if defined(__mmem_debug_enbale) && (__mmem_debug_enbale == 1)
#   undef calloc
#   define calloc(_counts, _item_size)  mmem_calloc(_counts, _item_size, __FILE__, __LINE__)
#   undef malloc
#   define malloc(_size)                mmem_alloc(_size, __FILE__, __LINE__)
#   undef free
#   define free(_addr)                  mmem_free(_addr, __FILE__, __LINE__)
#   undef realloc
#   define realloc(_addr, _size)        mmem_realloc(_addr, _size, __FILE__, __LINE__)
#endif/* defined(__mmem_debug_enbale) && (__mmem_debug_enbale == 1) */

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

#if defined(__cplusplus)
}
#endif/* defined(__cplusplus) */

#endif // !__MMEM_DEBUG_H__
