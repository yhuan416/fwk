#ifndef __MLIST_H__
#define __MLIST_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

#define MTRUE (1)
#define MFALSE (0)

/**
 * @brief 获取某个结构体在成员中的地址偏移量
 *
 * @param T 结构体类型
 * @param member 结构体成员名
 */
#define M_OFFSET_OF(T, member) (long)((char *)&(((T *)(0))->member))

/**
 * @brief 通过指定成员的地址获取结构体地址
 *
 * @param ptr 当前成员地址
 * @param T 结构体类型
 * @param member 结构体成员名
 */
#define M_GET_OBJECT(ptr, T, member) (T *)(((char *)(ptr)) - M_OFFSET_OF(T, member))

    typedef struct mlist
    {
        struct mlist *prev;
        struct mlist *next;
    } mlist;

#define MLIST_POISON ((struct mlist *)0)

    static inline void __mlist_add(mlist *head, mlist *node)
    {
        node->prev = head;
        node->next = head->next;
        head->next->prev = node;
        head->next = node;
    }

    static inline void __mlist_del(mlist *node)
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

/**
 * @brief 静态声明并初始化mlist节点
 *
 * @param list 要声明的节点名
 */
#define mlist_initialize(list) \
    mlist list = {&(list), &(list)}

    /**
     * @brief 初始化mlist节点
     *
     * @param head 待初始化的节点
     */
    static inline void mlist_init(mlist *head)
    {
        head->next = head;
        head->prev = head;
    }

    /**
     * @brief 反初始化某个节点
     *
     * @param head 待重初始化的节点
     */
    static inline void mlist_deinit(mlist *head)
    {
        __mlist_del(head);
        head->next = MLIST_POISON;
        head->prev = MLIST_POISON;
    }

    /**
     * @brief 判断以链表是否为空
     *
     * @param head mlist节点
     * @return int
     *              MTRUE  : 链表为空
     *              MFALSE : 链表不为空
     */
    static inline int mlist_empty(mlist *head)
    {
        return (head->next == head) ? (MTRUE) : (MFALSE);
    }

    /**
     * @brief 将某个节点插入到链表中
     *
     * @param head 待插入的链表
     * @param node 待插入的节点
     */
    static inline void mlist_add(mlist *head, mlist *node)
    {
        __mlist_add(head, node);
    }

    /**
     * @brief 将某个节点插入到链表中(尾插)
     *
     * @param head 待插入的链表
     * @param node 待插入的节点
     */
    static inline void mlist_add_tail(mlist *head, mlist *node)
    {
        __mlist_add(head->prev, node);
    }

    /**
     * @brief 删除某个节点
     *
     * @param node 待删除的节点
     */
    static inline void mlist_del(mlist *node)
    {
        __mlist_del(node);
        node->prev = MLIST_POISON;
        node->next = MLIST_POISON;
    }

/**
 * @brief 获取链表中的第一个成员
 *
 * @param head 链表头
 */
#define mlist_first(head) ((head)->next)

/**
 * @brief 遍历链表
 *
 * @param ptr 指针, 用于获取每个节点
 * @param head 链表头
 */
#define mlist_for_each(ptr, head) \
    for ((ptr) = (head)->next; (ptr) != (head); (ptr) = (ptr)->next)

/**
 * @brief 遍历链表
 *
 * @param ptr 指针, 用于获取每个节点
 * @param n 指针, 用于暂存下一个节点
 * @param head 链表头
 */
#define mlist_for_each_safe(ptr, n, head)                          \
    for ((ptr) = (head)->next, (n) = (ptr)->next; (ptr) != (head); \
         (ptr) = (n), (n) = (ptr)->next)

/**
 * @brief 获取链表中第一个节点所属的对象
 *
 * @param T 对象类型
 * @param head 链表头
 * @param member 节点成员名
 */
#define mlist_first_entry(T, head, member) M_GET_OBJECT((head)->next, T, member)

/**
 * @brief 遍历链表, 获取每个对象
 *
 * @param pos 指针, 用于获取每一个对象
 * @param T 对象类型
 * @param head 链表头
 * @param member 节点成员名
 */
#define mlist_for_each_entry(pos, T, head, member)      \
    for ((pos) = M_GET_OBJECT((head)->next, T, member); \
         &((pos)->member) != (head);                    \
         (pos) = M_GET_OBJECT((pos)->member.next, T, member))

/**
 * @brief 遍历链表, 获取每个对象
 *
 * @param pos 指针, 用于获取每一个对象
 * @param n 指针, 用于暂存下一个对象
 * @param T 对象类型
 * @param head 链表头
 * @param member 节点成员名
 */
#define mlist_for_each_entry_safe(pos, n, T, head, member) \
    for ((pos) = M_GET_OBJECT((head)->next, T, member),    \
        (n) = M_GET_OBJECT((pos)->member.next, T, member); \
         &((pos)->member) != (head);                       \
         (pos) = (n), (n) = M_GET_OBJECT((pos)->member.next, T, member))

#ifdef __cplusplus
}
#endif

#endif /* __MLIST_H__ */
