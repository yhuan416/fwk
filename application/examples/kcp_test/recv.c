#include "ikcp.h"
#include "common.h"

static int sockfd;
static struct sockaddr_in client_addr;
static socklen_t client_addr_len;

int udp_init(int port)
{
    int sockfd;
    struct sockaddr_in server_addr;

    // 创建套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("socket creation failed");
        return -1;
    }

    // 将套接字设置为非阻塞
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl GETFL failed");
        close(sockfd);
        return -1;
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl SETFL failed");
        close(sockfd);
        return -1;
    }

    // 绑定套接字
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int udp_output(const char *buf, int len, ikcpcb *kcp, void *user)
{
    ssize_t send_size;

    // 回显消息
    send_size = sendto(sockfd, buf, len, 0, (struct sockaddr *)&client_addr, client_addr_len);
    // printf("udp_output(%ld).\r\n", send_size);

    return 0;
}

int main(int argc, char const *argv[])
{
    char buffer[BUFFER_SIZE];
    ssize_t recv_size;
    int i;

    char message[BUFFER_SIZE];
    int len;

    sockfd = udp_init(SERVER_PORT);
    if (sockfd < 0)
    {
        return -1;
    }
    client_addr_len = sizeof(client_addr);

    ikcpcb *kcp = ikcp_create(CONV, NULL);
    ikcp_setoutput(kcp, udp_output);

    while (1)
    {
        isleep(100);

        // 更新 kcp 状态
        ikcp_update(kcp, iclock());

        // 接收消息
        recv_size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                             (struct sockaddr *)&client_addr, &client_addr_len);
        if (recv_size > 0)
        {
            // 处理收到的消息
            ikcp_input(kcp, buffer, recv_size);
        }

        // int ikcp_recv(ikcpcb *kcp, char *buffer, int len);
        len = ikcp_recv(kcp, message, BUFFER_SIZE);
        if (len > 0)
        {
            message[len] = '\0'; // 确保字符串终止

            // 处理收到的消息
            printf("Received: %s\n", message);

            // 发送消息
            ikcp_send(kcp, message, len);

            if (strcmp(message, "exit") == 0)
            {
                // 等待1s，确保消息发送完毕
                i = 10;
                while (i--)
                {
                    isleep(100);
                    // 更新 kcp 状态
                    ikcp_update(kcp, iclock());
                }
                ikcp_flush(kcp);

                break;
            }
        }
    }

    ikcp_release(kcp);
    close(sockfd);

    return 0;
}
