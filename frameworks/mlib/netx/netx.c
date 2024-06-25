#include "netx.h"

#include <net/if.h>

#include <sys/types.h>
#include <ifaddrs.h>

#define SOCK_DEFAULT 0

#define SOL_TCP 6 /* TCP level, TCP相关选项 */

#if !defined(print_level)
#define print_level 2
#endif
#if !defined(print_err)
#define print_err         \
    if (print_level >= 1) \
    printf
#define print_warn        \
    if (print_level >= 2) \
    printf
#define print_tag         \
    if (print_level >= 3) \
    printf
#define print_info        \
    if (print_level >= 4) \
    printf
#define print_debug       \
    if (print_level >= 5) \
    printf
#define print_detail      \
    if (print_level >= 6) \
    printf
#endif /* !defined(print_err) */

#ifndef netx_fcntl
#define netx_fcntl fcntl
#define netx_close(fd) close(fd)
#define netx_errno errno
#define netx_eintr EINTR
#define netx_ewouldblock EWOULDBLOCK
#define netx_econnreset ECONNRESET
#endif // end of netx_close

#if !defined(_epoll)

typedef struct netx_item
{
    struct netx_item *hash_next; /*!< hash next */
    struct netx_item *next;      /*!< next item */
    struct netx_item *prev;      /*!< prev item */
    long fd;                     /*!< fd, socket */
    struct netx_event event;     /*!< event */
} _netx_item;

/* copy from mlock.h */
#define mlock_disable (1)
#if defined(mlock_disable) && mlock_disable /* single thread, don't need lock */

#define mlock_simple void *
#define mlock_simple_init(lock)
#define mlock_simple_uninit(lock)
#define mlock_simple_wait(lock)
#define mlock_simple_release(lock)

#else /* just for multi thread, but not multi process */

#ifndef mosal_lock_simple
#error "Platform un-support mlock, You need add lock function and macro"
#endif

#define mlock_simple mosal_lock_simple
#define mlock_simple_init mosal_lock_simple_init
#define mlock_simple_uninit mosal_lock_simple_uninit
#define mlock_simple_wait(_lock) mosal_lock_simple_wait_directly(_lock)
#define mlock_simple_release(_lock) mosal_lock_simple_release_directly(_lock)

#endif

/* polls handle list */
#define netx__pool_magic (*(long *)"ntxp    ")
typedef struct netx_poll
{
    char magic[sizeof(long)];        /*!< magic */
    unsigned long max_counts;        /*!< max counts */
    unsigned long counts;            /*!< fd counts */
    unsigned long hash_size;         /*!< hash counts */
    mlock_simple lock;               /*!< lock object */
    struct netx_item *free_list;     /*!< free list */
    struct netx_item *used_list;     /*!< used list */
    struct netx_item *hash_table[1]; /*!< hash tables */
} _netx_poll;
#define netx__poll_hash_size(max_counts) ((((max_counts) >> 2) << 2) + 3)
#define netx__poll_size(max_counts) (sizeof(struct netx_poll) + (sizeof(struct netx_item) * (max_counts)) + (sizeof(unsigned long) * (netx__poll_hash_size(max_counts) - 1)))

static struct
{
    unsigned long counts;     /*!< poll counts */
    unsigned long max_counts; /*!< max poll counts */
    struct netx_poll **list;
} netx__polls = {0};

long netx_create(unsigned long max_counts)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_create(max_counts[%ld])"
#define func_format() max_counts

    unsigned long i;
    print_info("info: " func_format_s ". %s:%d\r\n", func_format(), __FILE__, __LINE__);
    if (0 == max_counts)
    { /* invalid max counts */
        print_err("err: " func_format_s " failed with invalid param. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        return -1;
    }

    if (max_counts > FD_SETSIZE)
    {
        print_warn("warn: " func_format_s " counts ==> %d. %s:%d\r\n", func_format(), FD_SETSIZE, __FILE__, __LINE__);
        max_counts = FD_SETSIZE;
    }

    if (netx__polls.counts >= netx__polls.max_counts)
    { /* need realloc polls */
        unsigned long list_size = sizeof(netx__polls.list[0]) * (netx__polls.max_counts + 16);
        struct netx_poll **list = (struct netx_poll **)(netx__polls.list ? realloc(netx__polls.list, list_size) : malloc(list_size));
        if (NULL == list)
        {
            print_err("err: " func_format_s " failed when malloc(%ld) poll-list. %s:%d\r\n",
                      func_format(), list_size, __FILE__, __LINE__);
            return -3;
        }
        memset(&list[netx__polls.max_counts], 0, sizeof(netx__polls.list[0]) * 16);
        netx__polls.list = list;
        netx__polls.max_counts += 16;
    }

    /* alloc a poll */
    for (i = 0; i < netx__polls.max_counts; ++i)
    {
        if (NULL == netx__polls.list[i])
        { /* found a position */
            long handle = i + 1;
            unsigned long poll_size = netx__poll_size(max_counts);
            struct netx_poll *poll_obj = (struct netx_poll *)malloc(poll_size);
            if (NULL == (netx__polls.list[i] = poll_obj))
            {
                print_err("err: " func_format_s " failed when malloc(%ld) poll. %s:%d\r\n",
                          func_format(), poll_size, __FILE__, __LINE__);
                return -4;
            }

            /* init pool */
            memset(poll_obj, 0, poll_size);
            *(long *)&poll_obj->magic = netx__pool_magic;
            mlock_simple_init(&poll_obj->lock);
            poll_obj->max_counts = max_counts;
            poll_obj->hash_size = netx__poll_hash_size(max_counts);
            poll_obj->free_list = (struct netx_item *)&poll_obj->hash_table[poll_obj->hash_size];
            for (i = 0; i < max_counts; ++i)
            {
                poll_obj->free_list[i].hash_next = &poll_obj->free_list[i + 1];
            }
            poll_obj->free_list[max_counts - 1].hash_next = NULL;
            netx__polls.counts++;
            return handle;
        }
    }

    print_err("err: " func_format_s " failed inner error. %s:%d\r\n", func_format(), __FILE__, __LINE__);
    return -5;
}

long netx_destroy(long poll)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_destroy(poll[%ld{%p{magic[%4.4s], counts[%ld], max_counts[%ld]}}])"
#define func_format() poll, poll_obj, poll_obj ? (char *)&poll_obj->magic : 0, poll_obj ? poll_obj->counts : 0, \
                      poll_obj ? poll_obj->max_counts : 0

    struct netx_poll *poll_obj = ((0 < poll) && ((unsigned long)poll <= netx__polls.max_counts)) ? netx__polls.list[poll - 1] : NULL;
    print_info("info: " func_format_s ". %s:%d\r\n", func_format(), __FILE__, __LINE__);
    /* check poll */
    if (NULL == poll_obj)
    {
        print_err("err: " func_format_s " failed with invalid poll handle. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        return -1;
    }
    if ((netx__pool_magic != *(long *)&poll_obj->magic) || (poll_obj->max_counts < poll_obj->counts))
    {
        print_err("err: " func_format_s " failed with invalid poll. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        return -2;
    }

    mlock_simple_wait(&poll_obj->lock);
    if (poll_obj->counts)
    {
        print_warn("warn: " func_format_s " still have item it. %s:%d\r\n", func_format(), __FILE__, __LINE__);
    }

    /* free it */
    netx__polls.list[poll - 1] = NULL;
    --netx__polls.counts;
    mlock_simple_release(&poll_obj->lock);
    mlock_simple_uninit(&poll_obj->lock);
    free(poll_obj);

    if (0 == netx__polls.counts)
    { /* clear resouce */
        free(netx__polls.list);
        netx__polls.list = NULL;
        netx__polls.counts = netx__polls.max_counts = 0;
    }
    return 0;
}

long netx_ctl(long poll, unsigned long operation, long fd, struct netx_event *event)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_ctl(poll[%ld{%p{magic[%4.4s], counts[%ld], max_counts[%ld]}}], operation[%ld], fd[%ld], event[%p])"
#define func_format() poll, poll_obj, poll_obj ? (char *)&poll_obj->magic : 0, poll_obj ? poll_obj->counts : 0, \
                      poll_obj ? poll_obj->max_counts : 0, operation, fd, event

    struct netx_item **item_save_pt, *item;
    struct netx_poll *poll_obj = ((0 < poll) && ((unsigned long)poll <= netx__polls.max_counts)) ? netx__polls.list[poll - 1] : NULL;
    print_detail("detail: " func_format_s ". %s:%d\r\n", func_format(), __FILE__, __LINE__);
    if ((NULL == poll_obj) || (fd < 0) || ((netx_ctl_del != operation) && (NULL == event)))
    {
        print_err("err: " func_format_s " failed with invalid param. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        return -1;
    }

    mlock_simple_wait(&poll_obj->lock);

    /* get item */
    item = *(item_save_pt = &poll_obj->hash_table[((unsigned long)fd) % poll_obj->hash_size]);
    while (item && (fd != item->fd))
    { /* search item */
        item = *(item_save_pt = &item->hash_next);
    }

    /* do operation */
    switch (operation)
    {
    case netx_ctl_mod:
    {
        if (NULL == item)
        {
            mlock_simple_release(&poll_obj->lock);
            print_err("err: " func_format_s " failed because fd not exist. %s:%d\r\n", func_format(), __FILE__, __LINE__);
            return -2;
        }
        item->event = *event;
        break;
    }
    case netx_ctl_del:
    {
        if (NULL == item)
        {
            mlock_simple_release(&poll_obj->lock);
            print_err("err: " func_format_s " failed because fd not exist. %s:%d\r\n", func_format(), __FILE__, __LINE__);
            return -3;
        }
        /* set fields */
        item->event.events = 0;
        item->fd = 0;
        /* delete from used list */
        if (item == poll_obj->used_list)
        { /* fist used item */
            poll_obj->used_list = (item == item->prev) ? NULL : item->next;
        }
        item->prev->next = item->next;
        item->next->prev = item->prev;
        item->prev = item->next = NULL;
        /* delete from hash */
        *item_save_pt = item->hash_next;
        /* add to free list */
        item->hash_next = poll_obj->free_list;
        poll_obj->free_list = item;
        /* dec counts */
        --poll_obj->counts;
        break;
    }
    case netx_ctl_add:
    {
        if (item)
        {
            mlock_simple_release(&poll_obj->lock);
            print_err("err: " func_format_s " failed because fd exist. %s:%d\r\n", func_format(), __FILE__, __LINE__);
            return -4;
        }
        else if (NULL == poll_obj->free_list)
        {
            mlock_simple_release(&poll_obj->lock);
            print_err("err: " func_format_s " failed because poll is full. %s:%d\r\n", func_format(), __FILE__, __LINE__);
            return -5;
        }
        /* del from free list */
        item = poll_obj->free_list;
        poll_obj->free_list = item->hash_next;
        /* add to hash */
        item->hash_next = *item_save_pt;
        *item_save_pt = item;
        /* add to from used list */
        if (NULL == poll_obj->used_list)
        { /* fist used item */
            poll_obj->used_list = item->prev = item->next = item;
        }
        else
        {
            item->next = poll_obj->used_list;
            item->prev = poll_obj->used_list->prev;
            poll_obj->used_list->prev->next = item;
            poll_obj->used_list->prev = item;
        }
        /* set fields */
        item->event = *event;
        item->fd = fd;
        /* inc counts */
        ++poll_obj->counts;
        break;
    }
    default:
    {
        mlock_simple_release(&poll_obj->lock);
        print_err("err: " func_format_s " failed unknown operation. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        return -1;
    }
    }

    mlock_simple_release(&poll_obj->lock);
    return 0;
}

long netx_wait(long poll, struct netx_event *events, unsigned long max_events_counts, unsigned long timeout)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_wait(poll[%ld{%p{magic[%4.4s], counts[%ld], max_counts[%ld]}}], events[%p], max_events_counts[%ld], timeout[%ld])"
#define func_format() poll, poll_obj, poll_obj ? (char *)&poll_obj->magic : 0, poll_obj ? poll_obj->counts : 0, \
                      poll_obj ? poll_obj->max_counts : 0, events, max_events_counts, timeout

    struct netx_poll *poll_obj = ((0 < poll) && ((unsigned long)poll <= netx__polls.max_counts)) ? netx__polls.list[poll - 1] : NULL;
    //    print_detail("detail: "func_format_s". %s:%d\r\n", func_format(), __FILE__, __LINE__);
    if ((NULL == poll_obj) || (NULL == events) || (0 == max_events_counts))
    {
        print_err("err: " func_format_s " failed with invalid param. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        return -1;
    }

    mlock_simple_wait(&poll_obj->lock);

    if (0 == poll_obj->counts)
    { /* have not items */
        mlock_simple_release(&poll_obj->lock);
        sleep(timeout / 1000);
        return 0;
    }
    else
    {
        struct timeval tv;
        long ret;
        fd_set fs_in, fs_out, fs_et;
        long in_counts = 0, out_counts = 0, et_counts = 0, max_sock = SOCK_DEFAULT, max_counts = 0;
        struct netx_item *item_next, *item = poll_obj->used_list;
        FD_ZERO(&fs_in);
        FD_ZERO(&fs_out);
        FD_ZERO(&fs_et);
        /* set time-out */
        tv.tv_usec = 1000 * (timeout % 1000);
        tv.tv_sec = timeout / 1000;

        /* set sets */
        do
        {
            if (item->event.events & netx_event_in)
            {
                FD_SET(item->fd, &fs_in);
                max_sock = (max_sock < item->fd) ? item->fd : max_sock;
                if ((++in_counts) > max_counts)
                {
                    max_counts = in_counts;
                };
            }
            if (item->event.events & netx_event_out)
            {
                FD_SET(item->fd, &fs_out);
                max_sock = (max_sock < item->fd) ? item->fd : max_sock;
                if ((++out_counts) > max_counts)
                {
                    max_counts = out_counts;
                };
            }
            if (item->event.events & netx_event_et)
            {
                FD_SET(item->fd, &fs_et);
                max_sock = (max_sock < item->fd) ? item->fd : max_sock;
                if ((++et_counts) > max_counts)
                {
                    max_counts = et_counts;
                };
            }
        } while ((item = item->next) != poll_obj->used_list);

        mlock_simple_release(&poll_obj->lock);

        if (SOCK_DEFAULT == max_sock)
        { /* have not item to watch */
            sleep(timeout / 1000);
            return 0;
        }

        /* check data */
        ret = select(max_sock + 1, &fs_in, &fs_out, &fs_et, &tv);
        if (0 > ret)
        {
#if defined(__APPLE__) || defined(__rtthread__)
            /* darwin平台下，假如在select阻塞过程中，select等待得句柄set中某个fd被close，那么就会返回错误，直接忽略即可,
             * linux平台会直接不管这个socket */
            if (netx_errno == EBADF)
            {
                return 0;
            }
#endif
            print_err("err: " func_format_s " failed when select() ret[%ld] with syserrno[%d:%s]. %s:%d\r\n",
                      func_format(), ret, netx_errno, strerror(netx_errno), __FILE__, __LINE__);
            return -2;
        }

        mlock_simple_wait(&poll_obj->lock);

        if ((0 < ret) && poll_obj->used_list)
        {
            unsigned long event, event_counts = 0;
            print_detail("detail: " func_format_s " ret[%ld] %s:%d\r\n", func_format(), ret, __FILE__, __LINE__);
            item = poll_obj->used_list;
            do
            {
                item_next = item->next;
                if (NULL == item_next)
                { /* inner error */
                    print_err("err: " func_format_s " meeting a invalid item[%p{fd[%ld]}], event[%ld] inner error, maybe need lock. %s:%d\r\n",
                              func_format(), item, item->fd, event, __FILE__, __LINE__);
                    mlock_simple_release(&poll_obj->lock);
                    return event_counts;
                }
                /* check event */
                event = 0;
                if ((item->event.events & netx_event_in) && FD_ISSET(item->fd, &fs_in))
                {
                    event |= netx_event_in;
                }
                if ((item->event.events & netx_event_out) && FD_ISSET(item->fd, &fs_out))
                {
                    event |= netx_event_out;
                }
                if ((item->event.events & netx_event_et) && FD_ISSET(item->fd, &fs_et))
                {
                    event |= netx_event_et;
                }

                /* out */
                if (event)
                {
                    events[event_counts].data.ptr = item->event.data.ptr;
                    events[event_counts].events = event;
                    print_detail("detail: " func_format_s " fd[%ld] events[%ld] %s:%d\r\n",
                                 func_format(), item->fd, event, __FILE__, __LINE__);
                    if ((++event_counts) == max_events_counts)
                    { /* check event is full now */
                        mlock_simple_release(&poll_obj->lock);
                        return event_counts;
                    }
                }
            } while ((item = item_next) != poll_obj->used_list);

            /* let all connection be check */
            poll_obj->used_list = poll_obj->used_list->next;

            mlock_simple_release(&poll_obj->lock);
            return event_counts;
        }

        mlock_simple_release(&poll_obj->lock);
        return 0;
    }
}

#endif

/*! get socket name, used it at once(temp data just for 2 caller), output as "192.168.1.2<->202.201.0.1" */
const char *netx_stoa(long fd)
{
    static char name_str[2][128];
    static long index;
    struct sockaddr_in si_local = {0}, si_peer = {0};
    long ret, cur_index = (++index) & 0x01;

    name_str[0][sizeof(name_str[0]) - 1] = (name_str[1][sizeof(name_str[1]) - 1] = 0);

    if (0 < fd)
    { /* valid socket */
        socklen_t si_local_size = sizeof(si_local), si_peer_sz = sizeof(si_peer);
        getsockname(fd, (struct sockaddr *)&si_local, &si_local_size);
        getpeername(fd, (struct sockaddr *)&si_peer, &si_peer_sz);
    } /* sizeof(name_str) - 1 for cross platform */

    ret = snprintf(name_str[cur_index], sizeof(name_str[cur_index]) - 1, "%s:%d", inet_ntoa(si_local.sin_addr), ntohs(si_local.sin_port));
    if (si_peer.sin_port)
    { /* have peer data */
        ret += snprintf(&name_str[cur_index][ret], sizeof(name_str[cur_index]) - ret - 1, "<->%s:%d", inet_ntoa(si_peer.sin_addr), ntohs(si_peer.sin_port));
    }

    name_str[cur_index][ret] = 0;
    return (const char *)&name_str[cur_index];
}

long netx_open(long type, struct in_addr *ip, unsigned long port, unsigned long flag)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_open(type[%ld], ip[%p{%s}], port[%ld], flag[0x%lx])"
#define func_format() type, ip, ip ? inet_ntoa(*ip) : "", port, flag

    long fd;
    struct sockaddr_in addr;

    print_debug("debug: " func_format_s ". %s:%d\r\n", func_format(), __FILE__, __LINE__);

    /* create socket */
    if (0 > (fd = socket(AF_INET, type, 0)))
    { /* create socket failed */
        print_err("err: " func_format_s " failed when socket() with syserrno[%d]. %s:%d\r\n",
                  func_format(), netx_errno, __FILE__, __LINE__);
        return -1;
    }

    /* bind */
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs((unsigned short)port);
    addr.sin_addr.s_addr = ip ? ip->s_addr : 0;
    if (port && (flag & netx_open_flag_reuse_addr))
    {
        unsigned long reuse_addr = 1;
        if (0 > setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse_addr, sizeof(reuse_addr)))
        { /* set socket reuse opt failed */
            print_err("err: " func_format_s " failed when setsockopt(SO_REUSEADDR) with syserrno[%d]. %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
            netx_close(fd);
            return -1;
        }
    }

    if (netx_fcntl(fd, F_SETFL, netx_fcntl(fd, F_GETFL, 0) | O_NONBLOCK))
    { /* set sock to non-block mode failed. */
        print_err("err: " func_format_s " failed when set NONBLOCK[%x] with syserrno[%d]. %s:%d\r\n",
                  func_format(), O_NONBLOCK, netx_errno, __FILE__, __LINE__);
        netx_close(fd);
        return -1;
    }

    if (port || addr.sin_addr.s_addr)
    {
        if (0 > bind(fd, (const struct sockaddr *)&addr, sizeof(addr)))
        { /* bind socket failed */
            print_err("err: " func_format_s " failed when bind() with syserrno[%d]. %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
            netx_close(fd);
            return -1;
        }
    }

#ifdef __comm_linux__
    if ((SOCK_STREAM == type) && (0 > listen(fd, 65535)))
#else
    if ((SOCK_STREAM == type) && (0 > listen(fd, 32)))
#endif
    { /* listen failed */
        print_err("err: " func_format_s " failed when listen() with syserrno[%d]. %s:%d\r\n",
                  func_format(), netx_errno, __FILE__, __LINE__);
        netx_close(fd);
        return -1;
    }
    return fd;
}

long netx_connect_by_addr(struct sockaddr_in *addr, struct sockaddr_in *local_addr)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_connect_by_addr(addr[%p{%s:%d}], local_addr[%p{%s:%d})"
#define func_format() addr, addr ? strcpy(s1, inet_ntoa(addr->sin_addr)) : "", (int)(addr ? ntohs(addr->sin_port) : 0), \
                      local_addr, local_addr ? strcpy(s2, inet_ntoa(local_addr->sin_addr)) : "", (int)(local_addr ? ntohs(local_addr->sin_port) : 0)

    char s1[32], s2[32];
    long fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    print_debug("debug: " func_format_s " ret-fd[%ld]. %s:%d\r\n", func_format(), fd, __FILE__, __LINE__);

    /* create socket */
    if (0 > fd)
    { /* create socket failed */
        print_err("err: " func_format_s " failed when socket() with syserrno[%d]. %s:%d\r\n",
                  func_format(), netx_errno, __FILE__, __LINE__);
        return -1;
    }

    /* set non-block */
    if (netx_fcntl(fd, F_SETFL, netx_fcntl(fd, F_GETFL, 0) | O_NONBLOCK))
    { /* set sock to non-block mode failed. */
        print_err("err: " func_format_s " failed when fcntl(NONBLOCK) with syserrno[%d]. %s:%d\r\n",
                  func_format(), netx_errno, __FILE__, __LINE__);
        netx_close(fd);
        return -1;
    }

    if (local_addr)
    { /* need setting local port */
        unsigned reuse_addr = 1;
        if (0 > setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse_addr, sizeof(reuse_addr)))
        { /* set socket reuse opt failed */
            print_err("err: " func_format_s " failed when setsockopt(SO_REUSEADDR) with syserrno[%d]. %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
            netx_close(fd);
            return -1;
        }
        else if (0 > bind(fd, (const struct sockaddr *)local_addr, sizeof(*local_addr)))
        { /* bind socket failed */
            print_err("err: " func_format_s " failed when bind() with syserrno[%d]. %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
            netx_close(fd);
            return -1;
        }
    }

    /* connect */
    if ((0 > connect(fd, (struct sockaddr *)addr, sizeof(*addr))) && (EINPROGRESS != errno))
    { /* connect failed */
        print_err("info: " func_format_s " failed when connect() with syserrno[%d]. %s:%d\r\n",
                  func_format(), netx_errno, __FILE__, __LINE__);
        netx_close(fd);
        return -1;
    }

    return fd;
}

long netx_connect(char *host, unsigned long port, struct in_addr *local_ip, unsigned long local_port, struct sockaddr_in *addr)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_connect(host[%s], port[%ld], local_ip[%p{%s}], local_port[%ld], addr[%p])"
#define func_format() host, port, local_ip, local_ip ? inet_ntoa(*local_ip) : "", local_port, addr

    struct sockaddr_in addr_local;
    struct sockaddr_in addr_tmp;
    struct hostent *hostet = NULL;
    struct in_addr ip = {0};

    print_debug("debug: " func_format_s ". %s:%d\r\n", func_format(), __FILE__, __LINE__);

    if (0 == inet_aton(host, &ip))
    { /* not a ip host */
        print_err("err: " func_format_s " failed with invalid param[not support hostname]. "
                  "%s:%d\r\n",
                  func_format(), __FILE__, __LINE__);
        return -1;
    }

    addr = addr ? addr : &addr_tmp;
    addr->sin_family = AF_INET;
    addr->sin_port = htons((unsigned short)port);
    addr->sin_addr = ip;

    addr_local.sin_family = AF_INET;
    addr_local.sin_addr.s_addr = local_ip ? local_ip->s_addr : 0;
    addr_local.sin_port = ntohs((unsigned short)local_port);

    return netx_connect_by_addr(addr, local_port ? &addr_local : NULL);
}

long netx_accept(long fd, struct sockaddr_in *addr)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_accept(fd[%ld{%s}], addr[%p])"
#define func_format() fd, netx_stoa(fd), addr

    /* accept */
    struct sockaddr_in addr_tmp = {0};
    socklen_t addr_len = sizeof(struct sockaddr_in);
    long acpt_fd = accept(fd, (struct sockaddr *)(addr ? addr : &addr_tmp), &addr_len);
    if (0 > acpt_fd)
    {
        if (netx_errno && (netx_ewouldblock != netx_errno))
        {
            print_err("err: " func_format_s " failed when accept() == ret-socket[%ld{%s}] with net-errno[%d]. %s:%d\r\n",
                      func_format(), acpt_fd, netx_stoa(acpt_fd), netx_errno, __FILE__, __LINE__);
        }
        else
        {
            print_debug("debug: " func_format_s " block when accept() == ret-socket[%ld{%s}] with net-errno[%d]. %s:%d\r\n",
                        func_format(), acpt_fd, netx_stoa(acpt_fd), netx_errno, __FILE__, __LINE__);
        }
        return -1;
    }

    print_debug("debug: netx_accept(fd[%ld{%s}]) accept() == ret-socket[%ld{%s}]. %s:%d\r\n",
                fd, netx_stoa(fd), fd, netx_stoa(acpt_fd), __FILE__, __LINE__);

    /* set non-block */
    if (netx_fcntl(acpt_fd, F_SETFL, netx_fcntl(acpt_fd, F_GETFL, 0) | O_NONBLOCK))
    { /* set sock to non-block mode failed. */
        print_err("err: " func_format_s " failed when set NONBLOCK == ret-socket[%ld{%s}] with syserrno[%d]. %s:%d\r\n",
                  func_format(), acpt_fd, netx_stoa(acpt_fd), netx_errno, __FILE__, __LINE__);
        netx_close(acpt_fd);
        return -1;
    }

    return acpt_fd;
}

long netx_set_keepalive(long fd, long idle_ms, long interval_ms, long times)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_set_keepalive(fd[%ld], idle_ms[%ld], interval_ms[%ld], times[%ld])"
#define func_format() fd, idle_ms, interval_ms, times

    int keeplive_flag = 1;
    int keeplive_idle = (idle_ms ? idle_ms : 7200000) / 1000;
    int keeplive_interval = (interval_ms ? interval_ms : 5000) / 1000;
    int keeplive_times = times ? times : 5;
    long err_flag = 0, err_idle = 0, err_interval = 0, err_times = 0;

    if ((0 > (err_flag = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keeplive_flag, sizeof(keeplive_flag))))
#if defined(__linux__) && (!defined(__rtthread__))
        ||
        (keeplive_idle && (0 > (err_idle = setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (void *)&keeplive_idle, sizeof(keeplive_idle))))) ||
        (keeplive_interval && (0 > (err_interval = setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (void *)&keeplive_interval, sizeof(keeplive_interval))))) ||
        (keeplive_times && (0 > (err_times = setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (void *)&keeplive_times, sizeof(keeplive_times)))))
#endif
    )
    {
        print_err("err: " func_format_s " failed when setsockopt(%s) with err[%ld]. %s:%d\r\n", func_format(),
                  (err_flag ? "SO_KEEPALIVE" : (err_idle ? "TCP_KEEPIDLE" : (err_interval ? "TCP_KEEPINTVL" : "TCP_KEEPCNT"))),
                  (long)netx_errno, __FILE__, __LINE__);
        return -1;
    }

    print_debug("debug: " func_format_s " done. %s:%d\r\n", func_format(), __FILE__, __LINE__);
    return 0;
}

long netx_get_local_ip(struct in_addr *addr)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_get_local_ip(addr[%p])"
#define func_format() addr

    long ret = -1;
    long fd = 0;
    struct sockaddr_in sa_remote = {0};
    struct sockaddr_in sa_local = {0};
    socklen_t sa_local_len = sizeof(struct sockaddr);

    if (NULL == addr)
    {
        print_err("err: " func_format_s " failed with invalid param. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        return -1;
    }
    addr->s_addr = 0;

    /* normal way */
    sa_remote.sin_addr.s_addr = htonl(0x64646464);
    sa_remote.sin_family = AF_INET;
    sa_remote.sin_port = htons(65534);
    if ((0 > (fd = netx_open(SOCK_DGRAM, 0, 0, netx_open_flag_reuse_addr))) || connect(fd, (void *)&sa_remote, sizeof(sa_remote)))
    {
        print_err("err: " func_format_s " fail when %s. %s:%d\r\n", func_format(), (0 > fd) ? "netx_open" : "connect", __FILE__, __LINE__);
    }
    else
    {
        getsockname(fd, (void *)&sa_local, &sa_local_len);
        *addr = sa_local.sin_addr;
        print_detail("detail: " func_format_s " get local ip[%s] by connect(). %s:%d\r\n", func_format(), inet_ntoa(*addr), __FILE__, __LINE__);
    }

    if (0 <= fd)
    {
        netx_close(fd);
        fd = -1;
    }

    /* wince get by host name */
    if (0 == addr->s_addr)
    {
        char hostname[256] = {0};
        struct hostent *host = NULL;

        if (gethostname(hostname, 256))
        {
            print_err("err: " func_format_s " fail when gethostname() with net-errno[%d]. %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
        }
        else if (NULL == (host = gethostbyname(hostname)))
        {
            print_err("err: " func_format_s " fail when gethostbyname() with net-errno[%d]. %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
        }
        else if ((0 == ((struct in_addr *)*(host->h_addr_list))->s_addr) || (127 == *((unsigned char *)*(host->h_addr_list))))
        {
            print_detail("detail: " func_format_s " get unready ip[%s] by gethostname(), ignore it. %s:%d\r\n",
                         func_format(), inet_ntoa(*(struct in_addr *)*(host->h_addr_list)), __FILE__, __LINE__);
        }
        else
        {
            *addr = *((struct in_addr *)*host->h_addr_list);
            print_detail("detail: " func_format_s " get local ip[%s] by gethostname(). %s:%d\r\n", func_format(), inet_ntoa(*addr), __FILE__, __LINE__);
        }
    }

    /* linux get by ioctl(eth0) */
    if (addr->s_addr == 0)
    {
        struct ifreq ifr = {0};
        struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
        if (0 > (fd = socket(AF_INET, SOCK_DGRAM, 0)))
        {
            print_err("err: " func_format_s " fail when socket() with net-errno[%d]. %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
        }
        else
        {
            unsigned long i;
            char *if_list[] = {"eth0", "eth1"};
            for (i = 0; i < (sizeof(if_list) / sizeof(if_list[0])); ++i)
            {
                strcpy(ifr.ifr_name, if_list[i]);
                if (ioctl(fd, SIOCGIFADDR, &ifr))
                {
                    print_err("err: " func_format_s " fail when ioctl() with syserr[%s]. %s:%d\r\n",
                              func_format(), strerror(errno), __FILE__, __LINE__);
                }
                else
                {
                    *addr = sin->sin_addr;
                    print_detail("detail: " func_format_s " get if[%s] local ip[%s] by ioctl(). %s:%d\r\n",
                                 func_format(), if_list[i], inet_ntoa(*addr), __FILE__, __LINE__);
                    break;
                }
            }
            close(fd);
        }
    }

    if (addr->s_addr == 0)
    {
        print_err("err: " func_format_s " fail. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        return -2;
    }

    print_debug("debug: " func_format_s " ret[%s]. %s:%d\r\n", func_format(), inet_ntoa(*addr), __FILE__, __LINE__);
    return 0;
}

char *netx_get_addrinfo(char *ip, char *port, int ai_flag, int ai_family, int ai_sock, int ai_proto)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_get_addrinfo(ip[%s])"
#define func_format() ip

    long ret = -1;
    static char buf[2048] = {0};
    long buf_len = 0;
    struct addrinfo hints, *res = NULL, *res0 = NULL;
    struct sockaddr_in *addr_in4 = NULL;
    struct sockaddr_in6 *addr_in6 = NULL;
    long counts = 0;

    /* normal way */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = ai_family;
    hints.ai_socktype = ai_sock;
    hints.ai_flags = ai_flag;
    hints.ai_protocol = ai_proto;
    if (0 != (ret = getaddrinfo(ip, port, &hints, &res0)))
    {
        print_err("err: " func_format_s " failed when getaddrinfo, %s. %s:%d\r\n", func_format(), gai_strerror(ret), __FILE__, __LINE__);
        buf_len = sprintf(buf, "fail when getaddrinfo:%s", gai_strerror(ret));
        goto exit_label;
    }

    buf[0] = 0;
    for (res = res0; res; res = res->ai_next)
    {
        char temp[256] = {0};
        counts++;
        if (res->ai_family == AF_INET)
        {
            addr_in4 = (struct sockaddr_in *)res->ai_addr;
            inet_ntop(res->ai_family, &addr_in4->sin_addr, temp, sizeof(temp));
            buf_len += sprintf(&buf[buf_len], "%ld: flags:%ld, family:%ld, sock:%ld, proto:%ld, addr:%s, port:%d. \r\n", counts, (long)res->ai_flags, (long)res->ai_family, (long)res->ai_socktype, (long)res->ai_protocol, temp, ntohs(addr_in4->sin_port));
        }
        else if (res->ai_family == AF_INET6)
        {
            addr_in6 = (struct sockaddr_in6 *)res->ai_addr;
            inet_ntop(res->ai_family, &addr_in6->sin6_addr, temp, sizeof(temp));
            buf_len += sprintf(&buf[buf_len], "%ld: flags:%ld, family:%ld, sock:%ld, proto:%ld, addr:%s, port:%d. \r\n", counts, (long)res->ai_flags, (long)res->ai_family, (long)res->ai_socktype, (long)res->ai_protocol, temp, ntohs(addr_in6->sin6_port));
        }
    }

exit_label:
    if (res0)
    {
        freeaddrinfo(res0);
        res0 = NULL;
    }

    if (buf_len == 0)
    {
        sprintf(buf, "null");
    }
    return buf;
}

char *netx_get_ifaddr()
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_get_ifaddr([%s])"
#define func_format() ""

#if defined(_getifaddrs)
    long ret = -1;
    static char buf[2048] = {0};
    long buf_len = 0;
    struct ifaddrs *myaddrs, *ifa;
    struct sockaddr_in *addr_in4 = NULL;
    struct sockaddr_in6 *addr_in6 = NULL;
    long counts = 0;

    if (0 != (ret = getifaddrs(&myaddrs)))
    {
        print_err("err: " func_format_s " failed when getifaddrs. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        buf_len = sprintf(buf, "fail when getifaddrs");
        goto exit_label;
    }

    buf[0] = 0;
    for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next)
    {
        char temp[256] = {0};
        counts++;

        if (ifa->ifa_addr == NULL)
        {
            continue;
        }

        switch (ifa->ifa_addr->sa_family)
        {
        case AF_INET:
            addr_in4 = (struct sockaddr_in *)ifa->ifa_addr;
            inet_ntop(ifa->ifa_addr->sa_family, &addr_in4->sin_addr, temp, sizeof(temp));
            buf_len += sprintf(&buf[buf_len], "%ld, family:%ld, addr:%s. \r\n", counts, (long)ifa->ifa_addr->sa_family, temp);
            break;
        case AF_INET6:
            addr_in6 = (struct sockaddr_in6 *)ifa->ifa_addr;
            inet_ntop(ifa->ifa_addr->sa_family, &addr_in6->sin6_addr, temp, sizeof(temp));
            buf_len += sprintf(&buf[buf_len], "%ld, family:%ld, addr:%s. \r\n", counts, (long)ifa->ifa_addr->sa_family, temp);
            break;
        default:
            break;
        }
    }

exit_label:
    if (buf_len == 0)
    {
        sprintf(buf, "null");
    }
    return buf;
#else
    return "not support";
#endif
}

long netx_get_local_ip_2(struct netx_sockaddr *so_addr)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_get_local_ip_2(so_addr[%p])"
#define func_format() so_addr

    long ret = -1;
    struct sockaddr_in *addr_in4 = NULL, *addr_in4_temp = NULL;
    struct sockaddr_in6 *addr_in6 = NULL, *addr_in6_temp = NULL;

    if (so_addr == NULL)
    {
        print_err("err: " func_format_s " failed for invalid param. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        goto exit_label;
    }

#ifndef _getifaddrs
    memset(&so_addr->addr_v4, 0, sizeof(so_addr->addr_v4));
    if (netx_get_local_ip(&so_addr->addr_v4.sin_addr))
    {
        print_err("err: " func_format_s " failed when netx_get_local_ip. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        goto exit_label;
    }

    so_addr->addr_v4.sin_family = AF_INET;
    so_addr->addr_len = sizeof(struct sockaddr_in);
    memcpy(&so_addr->addr, &so_addr->addr_v4, so_addr->addr_len);
#else
    struct ifaddrs *myaddrs, *ifa;
    if (0 != (ret = getifaddrs(&myaddrs)))
    {
        print_err("err: " func_format_s " failed when getifaddrs. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        goto exit_label;
    }

    for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
        {
            continue;
        }
        switch (ifa->ifa_addr->sa_family)
        {
        case AF_INET:
            addr_in4_temp = (struct sockaddr_in *)ifa->ifa_addr;
            if (addr_in4 == NULL && addr_in4_temp->sin_addr.s_addr != 0x100007F)
            {
                addr_in4 = addr_in4_temp;
            }
            break;
        case AF_INET6:
            addr_in6_temp = (struct sockaddr_in6 *)ifa->ifa_addr;
            if (addr_in6 == NULL && !((addr_in6_temp->sin6_addr.s6_addr[0] == 0xfe && addr_in6_temp->sin6_addr.s6_addr[1] == 0x80) || addr_in6_temp->sin6_addr.s6_addr[0] == 0))
            {
                addr_in6 = addr_in6_temp;
            }
            break;
        default:
            break;
        }
    }

    if (addr_in4)
    {
        memcpy(&so_addr->addr_v4, addr_in4, sizeof(struct sockaddr_in));
    }

    if (addr_in6)
    {
        memcpy(&so_addr->addr_v6, addr_in6, sizeof(struct sockaddr_in6));
    }

    if (addr_in6 != NULL)
    {
        so_addr->addr_len = sizeof(struct sockaddr_in6);
        memcpy(&so_addr->addr, &so_addr->addr_v6, so_addr->addr_len);
    }
    else if (addr_in4 != NULL)
    {
        so_addr->addr_len = sizeof(struct sockaddr_in);
        memcpy(&so_addr->addr, &so_addr->addr_v4, so_addr->addr_len);
    }
#endif

    ret = 0;

exit_label:
    return ret;
}

long netx_get_sockaddr_by_addr(struct sockaddr *addr, struct netx_sockaddr *so_addr)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_get_local_sockaddr_ex( addr:%p, so_addr:%p )"
#define func_format() addr, so_addr

    long ret = -1;
    struct sockaddr_in6 *addr_in6 = NULL;
    char *temp = NULL;

    if (so_addr == NULL || NULL == addr)
    {
        print_err("err: " func_format_s " failed with invalid param. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        goto exit_label;
    }

    memset(so_addr, 0, sizeof(struct netx_sockaddr));
    if (addr->sa_family == AF_INET)
    {
        so_addr->addr_len = sizeof(struct sockaddr_in);

        memcpy(&so_addr->addr_v4, addr, sizeof(struct sockaddr_in));
        memcpy(&so_addr->addr, &so_addr->addr_v4, so_addr->addr_len);
    }
    else if (addr->sa_family == AF_INET6)
    {
        so_addr->addr_len = sizeof(struct sockaddr_in6);

        memcpy(&so_addr->addr_v6, addr, sizeof(struct sockaddr_in6));
        memcpy(&so_addr->addr, &so_addr->addr_v6, so_addr->addr_len);

        addr_in6 = (struct sockaddr_in6 *)addr;
        temp = ((char *)&addr_in6->sin6_addr) + sizeof(struct in6_addr) - sizeof(struct in_addr);
        so_addr->addr_v4.sin_family = AF_INET;

        memcpy(&so_addr->addr_v4.sin_addr, temp, sizeof(struct in_addr));
        so_addr->addr_v4.sin_port = addr_in6->sin6_port;
    }
    else
    {
        print_err("err: " func_format_s " failed with invalid param. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        goto exit_label;
    }

    ret = 0;

exit_label:
    return ret;
}

long netx_get_sockaddr_by_addr_v4(struct sockaddr_in *addr_v4, struct netx_sockaddr *so_addr)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_get_local_sockaddr_ex( addr_v4:%p, so_addr:%p )"
#define func_format() addr_v4, so_addr

    long ret = -1;
    char buf_ip[256] = {0};

    if (addr_v4 == NULL || NULL == so_addr)
    {
        print_err("err: " func_format_s " failed with invalid param. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        goto exit_label;
    }

    memset(so_addr, 0, sizeof(struct netx_sockaddr));
    so_addr->addr_len = sizeof(struct sockaddr_in);
    memcpy(&so_addr->addr_v4, addr_v4, so_addr->addr_len);
    memcpy(&so_addr->addr, addr_v4, so_addr->addr_len);
    so_addr->addr_v4.sin_family = 2;
    so_addr->addr.ss_family = 2;
    ret = 0;

exit_label:
    return ret;
}

long netx_get_sockaddr_by_string(char *ip, long ip_len, long port, struct netx_sockaddr *so_addr)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_get_sockaddr_by_string( ip:%s, ip_len:%ld, port:%ld, addr:%p )"
#define func_format() ip, ip_len, port, so_addr

    long ret = -1;
    long fd = 0;
    struct addrinfo hints, *res = NULL, *res0 = NULL;
    struct sockaddr_in *addr_in4 = NULL;
    struct sockaddr_in6 *addr_in6 = NULL;
    long counts = 0;
    char temp_ip[256] = {0};
    char temp_port[128] = {0};

    if (NULL == so_addr)
    {
        print_err("err: " func_format_s " failed with invalid param. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        goto exit_label;
    }

    memset(so_addr, 0, sizeof(struct netx_sockaddr));
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = 0;
    if (ip)
    {
        memcpy(temp_ip, ip, ip_len);
    }
    sprintf(temp_port, "%ld", port);

    if (0 != (ret = getaddrinfo(temp_ip, temp_port, &hints, &res0)))
    {
        print_err("err: " func_format_s " failed when getaddrinfo, %s. %s:%d\r\n", func_format(), gai_strerror(ret), __FILE__, __LINE__);
        goto exit_label;
    }

    for (res = res0; res; res = res->ai_next)
    {
        char temp[256] = {0};
        counts++;
        if (res->ai_family == AF_INET)
        {
            addr_in4 = (struct sockaddr_in *)res->ai_addr;
            inet_ntop(res->ai_family, &addr_in4->sin_addr, temp, sizeof(temp));
            print_tag("tag: " func_format_s "%ld: flags:%ld, family:%ld, sock:%ld, proto:%ld, addr:%s. %s:%d\r\n", func_format(), counts, (long)res->ai_flags, (long)res->ai_family, (long)res->ai_socktype, (long)res->ai_protocol, temp, __FILE__, __LINE__);
        }
        else if (res->ai_family == AF_INET6)
        {
            addr_in6 = (struct sockaddr_in6 *)res->ai_addr;
            inet_ntop(res->ai_family, &addr_in6->sin6_addr, temp, sizeof(temp));
            print_tag("tag: " func_format_s "%ld: flags:%ld, family:%ld, sock:%ld, proto:%ld, addr:%s. %s:%d\r\n", func_format(), counts, (long)res->ai_flags, (long)res->ai_family, (long)res->ai_socktype, (long)res->ai_protocol, temp, __FILE__, __LINE__);
        }
    }

    if (addr_in4 == NULL && addr_in6 == NULL)
    {
        print_err("err: " func_format_s " failed for null addr. %s:%d\r\n", func_format(), __FILE__, __LINE__);
        goto exit_label;
    }

    if (addr_in4)
    {
        addr_in4->sin_port = ntohs(port);
        memcpy(&so_addr->addr_v4, addr_in4, sizeof(struct sockaddr_in));
    }

    if (addr_in6)
    {
        addr_in6->sin6_port = ntohs(port);
        memcpy(&so_addr->addr_v6, addr_in6, sizeof(struct sockaddr_in6));
    }

    if (addr_in6 != NULL)
    {
        so_addr->addr_len = sizeof(struct sockaddr_in6);
        memcpy(&so_addr->addr, &so_addr->addr_v6, so_addr->addr_len);
    }
    else if (addr_in4 != NULL)
    {
        so_addr->addr_len = sizeof(struct sockaddr_in);
        memcpy(&so_addr->addr, &so_addr->addr_v4, so_addr->addr_len);
    }

exit_label:
    if (res0)
    {
        freeaddrinfo(res0);
        res0 = NULL;
    }
    return ret;
}

long netx_open_2(long type, char *ip, long ip_len, unsigned long port, unsigned long flag)
{
#undef func_format_s
#undef func_format
#define func_format_s "netx_open_2(type[%ld], ip[%p{%s}], port[%ld], flag[0x%lx])"
#define func_format() type, ip, ip ? ip : "", port, flag

    long fd;
    struct netx_sockaddr addr_temp = {0};
    struct netx_sockaddr addr_listen = {0};
    long ai_family = AF_INET6;
    long block_flag = 1;

    print_debug("debug: " func_format_s ". %s:%d\r\n", func_format(), __FILE__, __LINE__);

    /* Detect family */
    if (netx_get_sockaddr_by_string("54.157.82.107", strlen("54.157.82.107"), 7123, &addr_temp) || addr_temp.addr.ss_family == AF_INET)
    {
        ai_family = AF_INET;
    }

    /* create socket */
    if (0 > (fd = socket(ai_family, type, 0)))
    { /* create socket failed */
        print_err("err: " func_format_s " failed when socket() with syserrno[%d]. %s:%d\r\n",
                  func_format(), netx_errno, __FILE__, __LINE__);
        return -1;
    }

    if (ip || port)
    {
        struct sockaddr_in addr_v4 = {0};
        addr_v4.sin_family = AF_INET;
        addr_v4.sin_addr.s_addr = ip ? inet_addr(ip) : 0;
        if (addr_v4.sin_addr.s_addr == 0xffffff)
        {
            print_err("err: " func_format_s " hack for %s. %s:%d\r\n", func_format(), ip, __FILE__, __LINE__);
            addr_v4.sin_addr.s_addr = 0;
        }

        addr_v4.sin_port = ntohs((unsigned short)port);
        if (netx_get_sockaddr_by_addr_v4(&addr_v4, &addr_listen))
        { /* create socket failed */
            print_err("err: " func_format_s " failed when netx_get_sockaddr_by_addr_v4 with syserrno[%d], %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
            return -1;
        }
    }

    /* bind */
    if (port && (flag & netx_open_flag_reuse_addr))
    {
        unsigned long reuse_addr = 1;
        if (0 > setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse_addr, sizeof(reuse_addr)))
        { /* set socket reuse opt failed */
            print_err("err: " func_format_s " failed when setsockopt(SO_REUSEADDR) with syserrno[%d]. %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
            netx_close(fd);
            return -1;
        }
    }

    /* set non-block */
    if (netx_fcntl(fd, F_SETFL, netx_fcntl(fd, F_GETFL, 0) | O_NONBLOCK))
    { /* set sock to non-block mode failed. */
        print_err("err: " func_format_s " failed when set NONBLOCK with syserrno[%d]. %s:%d\r\n",
                  func_format(), netx_errno, __FILE__, __LINE__);
        netx_close(fd);
        return -1;
    }

    if (port || ip)
    {
        if (0 > bind(fd, (const struct sockaddr *)&addr_listen.addr, addr_listen.addr_len))
        { /* bind socket failed */
            print_err("err: " func_format_s " failed when bind() with syserrno[%d]. %s:%d\r\n",
                      func_format(), netx_errno, __FILE__, __LINE__);
            netx_close(fd);
            return -1;
        }
    }

#ifdef __comm_linux__
    if ((SOCK_STREAM == type) && (0 > listen(fd, 65535)))
#else
    if ((SOCK_STREAM == type) && (0 > listen(fd, 32)))
#endif
    { /* listen failed */
        print_err("err: " func_format_s " failed when listen() with syserrno[%d]. %s:%d\r\n",
                  func_format(), netx_errno, __FILE__, __LINE__);
        netx_close(fd);
        return -1;
    }
    return fd;
}

char *netx_sockaddr_ntop(struct netx_sockaddr *nso_addr)
{
    static char buf[512] = {0};
    char temp[256] = {0};

    if (nso_addr->addr.ss_family == AF_INET)
    {
        inet_ntop(nso_addr->addr.ss_family, &nso_addr->addr_v4.sin_addr, temp, sizeof(temp));
    }
    else if (nso_addr->addr.ss_family == AF_INET6)
    {
        inet_ntop(nso_addr->addr.ss_family, &nso_addr->addr_v6.sin6_addr, temp, sizeof(temp));
    }
    else
    {
        return "null:null";
    }

    sprintf(buf, "%s:%ld", temp, (long)((nso_addr->addr.ss_family == AF_INET) ? ntohs(nso_addr->addr_v4.sin_port) : ntohs(nso_addr->addr_v6.sin6_port)));
    return buf;
}
