#ifndef __NETX_H__
#define __NETX_H__

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <dirent.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/file.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <termios.h>
#include <assert.h>
#include <errno.h>
#include <netdb.h>

#if defined(__cplusplus)
extern "C"
{
#endif

#if defined(_epoll) /* use epoll */
#include <sys/epoll.h>

/* for struct netx_event's event */
#define netx_event_in EPOLLIN
#define netx_event_out EPOLLOUT
#define netx_event_et EPOLLET

/* for netx_ctl()'s operation */
#define netx_ctl_add (EPOLL_CTL_ADD)
#define netx_ctl_del (EPOLL_CTL_DEL)
#define netx_ctl_mod (EPOLL_CTL_MOD)

/* struct netx_event */
#define netx_event epoll_event

/* epoll functions */
#define netx_create(max_counts) epoll_create(max_counts)
#define netx_destroy(poll) close(poll)
#define netx_ctl(poll, operation, handle, event) epoll_ctl(poll, operation, handle, event)
#define netx_wait(poll, events, max_events_counts, timeout) epoll_wait(poll, events, max_events_counts, timeout)

#else /* !defined(_epoll), base select */

/* for struct netx_event's event */
#define netx_event_in (0x001)
#define netx_event_out (0x002)
#define netx_event_et (0x004)

/* for netx_ctl()'s operation */
#define netx_ctl_add (1)
#define netx_ctl_del (2)
#define netx_ctl_mod (3)

/* struct netx_event */
struct netx_event
{
    unsigned long events; /*!< events */
    union
    {
        void *ptr;
        long fd;
        unsigned u32;
        uint64_t u64;
    } data; /* data */
};

long netx_create(unsigned long max_counts);
long netx_destroy(long poll);
long netx_ctl(long poll, unsigned long operation, long fd, struct netx_event *event);
long netx_wait(long poll, struct netx_event *events, unsigned long max_events_counts, unsigned long timeout);

#endif /* !defined(_epoll) end */

#ifndef netx_close
#define netx_close(fd) close(fd)
#endif

    /*! get socket name, used it at once(temp data just for 2 caller), output as "192.168.1.2<->202.201.0.1" */
    const char *netx_stoa(long fd);

#define netx_open_flag_reuse_addr (0x01)
    long netx_open(long type,          /* socket()'s type, SOCK_STREAM... */
                   struct in_addr *ip, /* can be NULL, bind to 0.0.0.0 */
                   unsigned long port, /* host bit order */
                   unsigned long flag /* default 0 */);

    long netx_connect_by_addr(struct sockaddr_in *addr,
                              struct sockaddr_in *local_addr); /* local-addr, if NULL ignore */

    long netx_connect(char *host,
                      unsigned long port,
                      struct in_addr *local_ip,  /* can be NULL */
                      unsigned long local_port,  /* if 0, ignore local info, host bit order */
                      struct sockaddr_in *addr); /* [out] if NULL ignore*/

    long netx_accept(long fd, struct sockaddr_in *addr);

    long netx_set_keepalive(long fd, long idle_ms, long interval_ms, long times);

    long netx_get_local_ip(struct in_addr *addr);

    char *netx_get_addrinfo(char *ip, char *port, int ai_flag, int ai_family, int ai_sock, int ai_proto);

    char *netx_get_ifaddr();

    // netx_sockaddr

    typedef struct netx_sockaddr
    {
        struct sockaddr_storage addr;
        long addr_len;
        struct sockaddr_in addr_v4;
        struct sockaddr_in6 addr_v6;
    } _netx_sockaddr;

#define netx_sockaddr_get_port(so_addr) ntohs((so_addr) ? (((so_addr)->addr.ss_family == AF_INET) ? (so_addr)->addr_v4.sin_port : (so_addr)->addr_v6.sin6_port) : 0)
#define netx_sockaddr_set_port(so_addr, port)               \
    do                                                      \
    {                                                       \
        if ((so_addr))                                      \
        {                                                   \
            if (((so_addr)->addr.ss_family == AF_INET))     \
            {                                               \
                (so_addr)->addr_v4.sin_port = htons(port);  \
            }                                               \
            else                                            \
            {                                               \
                (so_addr)->addr_v6.sin6_port = htons(port); \
            }                                               \
        }                                                   \
    } while (0)

    extern long netx_get_local_ip_2(struct netx_sockaddr *so_addr);

    extern long netx_get_sockaddr_by_addr(struct sockaddr *addr, struct netx_sockaddr *so_addr);

    extern long netx_get_sockaddr_by_addr_v4(struct sockaddr_in *addr_v4, struct netx_sockaddr *so_addr);

    extern long netx_get_sockaddr_by_string(char *ip, long ip_len, long port, struct netx_sockaddr *so_addr);

    extern long netx_open_2(long type, char *ip, long ip_len, unsigned long port, unsigned long flag);

    extern char *netx_sockaddr_ntop(struct netx_sockaddr *nso_addr);

#if defined(__cplusplus)
}
#endif

#endif
