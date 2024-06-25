#include "stdio.h"

#include "netx.h"

#define SERVER_PORT (8888)

int main(int argc, char const *argv[])
{
    struct sockaddr_in addr = {0};

    long fd = netx_connect("127.0.0.1",
                           SERVER_PORT,
                           NULL,  /* can be NULL */
                           0,     /* if 0, ignore local info, host bit order */
                           NULL); /* [out] if NULL ignore*/
    if (fd < 0) {
        printf("netx_connect failed.\r\n");
        return -1;
    }

    printf("fd: %ld, %s\r\n", fd, netx_stoa(fd));

    write(fd, "hello", 5);

    netx_close(fd);

    return 0;
}
