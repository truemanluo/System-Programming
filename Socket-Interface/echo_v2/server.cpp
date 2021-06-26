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

int main()
{
    // 0. 初始化getaddrinfo
    int status; // getaddrinfo返回的状态
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof hints); // 初始化hints

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // 当node == NULL时表示返回的socket地址将以通配符的形式接受连接请求

    if ((status = getaddrinfo(NULL, PORT, &hints, &result)) < 0) {
        ERR_EXIT(gai_strerror(status));
    }

    // 1. 创建socket
    int listenfd;
    struct addrinfo *p;
    for (p = result; p != NULL; p = p->ai_next)    
    {
        // socket创建失败
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server : socket");
            continue;
        }
        // 处理端口被占用的情况
        int yes = 1;
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            ERR_EXIT("setsockopt");
        }
    
    // 2. 绑定

        // 绑定文件描述符和socket（ip address: port）
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == -1) { // 一次绑定失败
            close(listenfd); // 释放文件描述符
            perror("server : bind");
            continue;
        }

        break; // 绑定成功
    }

    // 3. 监听
    int listen_status;
    // listen（告诉内核当前的文件描述符是被服务器使用的（默认是客户端））
    if ((listen_status = listen(listenfd, SOMAXCONN)) == -1) {
        ERR_EXIT("listen failed.");
    }

    freeaddrinfo(result);

    // 5. 数据交换
    char recvbuf[1024]; // 接收缓冲区

    // 4. 创建连接
    int connfd;
    struct sockaddr_in peeraddr;
    socklen_t peerlen = sizeof peeraddr;
    pid_t pid;
    while (1)
    {        
        if ((connfd = accept(listenfd, (struct sockaddr *) &peeraddr, &peerlen)) < 0) {
            ERR_EXIT("accept");
        }
        printf("ip = %s, ", inet_ntoa(peeraddr.sin_addr));
        printf("port = %d\n", ntohs(peeraddr.sin_port));
        std::cout << "connfd: " << connfd << std::endl;
        if ((pid = fork()) < 0) {
            ERR_EXIT("fork");
        }

        // 在父进程中处理监听
        if (pid != 0) {
            close(connfd);
        }
        // 在子进程中进行数据传输
        else {
            close(listenfd);
            while (1) {
                memset(recvbuf, 0, sizeof recvbuf);
                int ret = read(connfd, recvbuf, sizeof recvbuf);
                fputs(recvbuf, stdout);
                write(connfd, recvbuf, ret);
            }
            
        }
    }
    // 6. 断开连接
    close(connfd);
    close(listenfd);
    return 0;
}