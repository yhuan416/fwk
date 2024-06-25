#include "stdio.h"

#include "netx.h"

#define SERVER_PORT (8888)

void handleClient(long fd)
{
    printf("handleClient: %ld\r\n", fd);

    char buf[1024] = {0};

    int len = read(fd, buf, 1024);
    if (len < 0)
    {
        printf("read failed.\r\n");
        netx_close(fd);
        return;
    }

    printf("read: %s\r\n", buf);

    netx_close(fd);
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in addr = {0};

    long fd = netx_open(SOCK_STREAM, /* socket()'s type, SOCK_STREAM... */
                        NULL,        /* can be NULL, bind to 0.0.0.0 */
                        SERVER_PORT,        /* host bit order */
                        netx_open_flag_reuse_addr /* default 0 */);
    if (fd < 0)
    {
        printf("netx_open failed.\r\n");
        return -1;
    }

    int epoll = netx_create(1);
    if (epoll < 0)
    {
        printf("netx_create failed.\r\n");
        return -1;
    }

    struct netx_event event = {0};
    event.events = netx_event_in;
    event.data.fd = fd;
    if (netx_ctl(epoll, netx_ctl_add, fd, &event) < 0)
    {
        printf("netx_ctl failed.\r\n");
        return -1;
    }

    struct netx_event events[1] = {0};
    while (1)
    {
        int num = netx_wait(epoll, events, 1, -1);
        if (num < 0)
        {
            printf("netx_wait failed.\r\n");
            break;
        }

        for (int i = 0; i < num; i++)
        {
            if (events[i].data.fd == fd)
            {
                long client = netx_accept(fd, &addr);
                if (client < 0)
                {
                    printf("netx_accept failed.\r\n");
                    break;
                }

                handleClient(client);
            }
        }
    }

    netx_destroy(epoll);

    netx_close(fd);

    return 0;
}
