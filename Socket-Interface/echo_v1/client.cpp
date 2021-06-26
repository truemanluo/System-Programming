#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>

#define PORT "3490"
#define ERR_EXIT(m) do {  perror(m);  exit(EXIT_FAILURE); } while(0);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: hostname\n");
        exit(EXIT_FAILURE);
    }
    // 0. 初始化getaddrinfo
    int status; // getaddrinfo返回的状态
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof hints); // 初始化hints

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_PASSIVE; // 客户端需要显示指明连接的主机名

    if ((status = getaddrinfo(argv[1], PORT, &hints, &result)) < 0) {
        ERR_EXIT(gai_strerror(status));
    }

    // 1. 创建socket
    int clientfd;
    struct addrinfo *p;
    for (p = result; p != NULL; p = p->ai_next)    
    {
        // socket创建失败
        if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client : socket\n");
            continue;
        }
    
    // 2. connect连接
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(clientfd);
            perror("client: connect\n");
            continue;
        }

        break; // 连接成功
    }

    if (p == NULL) {
        ERR_EXIT("connect failed.\n");
    }

    std::cout << "connection made!" << std::endl;

    // 3. 发送数据
    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    while (fgets(sendbuf, sizeof sendbuf, stdin) != NULL) {
        write(clientfd, sendbuf, strlen(sendbuf)); // 将sendbuf写入缓冲区
        read(clientfd, recvbuf, sizeof recvbuf);
        fputs(recvbuf, stdout); // 打印接受到的信息

        memset(recvbuf, 0, sizeof recvbuf);
        memset(sendbuf, 0, sizeof sendbuf);

    }

    freeaddrinfo(result);

    // 4. 关闭套接口
    close(clientfd);

    
    return 0;
}