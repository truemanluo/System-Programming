#include "sysutils.h"
#define MAXFDSIZE 2048

using namespace std;

struct packet
{
    int len;
    char buf[1024];
};

#define ERR_EXIT(m) \
        do  \
        {   \
            perror(m);  \
            exit(EXIT_FAILURE); \
        } while(0);

ssize_t readn(int fd, void *buf, size_t count)
{
    size_t nleft = count;   // 剩余字节数
    ssize_t nread;
    char *bufp = (char*) buf;

    while (nleft > 0)
    {
        nread = read(fd, bufp, nleft);
        if (nread < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return  -1;
        } else if (nread == 0)
        {
            return count - nleft;
        }

        bufp += nread;
        nleft -= nread;
    }
    return count;
}

ssize_t writen(int fd, const void *buf, size_t count)
{
    size_t nleft = count;
    ssize_t nwritten;
    char* bufp = (char*)buf;

    while (nleft > 0)
    {
        if ((nwritten = write(fd, bufp, nleft)) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return -1;
        }
        else if (nwritten == 0)
        {
            continue;
        }
        bufp += nwritten;
        nleft -= nwritten;
    }
    return count;
}

ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
    while (1)
    {
        int ret = recv(sockfd, buf, len, MSG_PEEK); // 查看传入消息
        if (ret == -1 && errno == EINTR)
        {
            continue;
        }
        return ret;
    }
}

ssize_t readline(int sockfd, void *buf, size_t maxline)
{
    int ret;
    int nread;
    char *bufp = (char*)buf;    // 当前指针位置
    int nleft = maxline;
    while (1)
    {
        ret = recv_peek(sockfd, buf, nleft);
        if (ret < 0)
        {
            return ret;
        }
        else if (ret == 0)
        {
            return ret;
        }
        nread = ret;
        int i;
        for (i = 0; i < nread; i++)
        {
            if (bufp[i] == '\n')
            {
                ret = readn(sockfd, bufp, i+1);
                if (ret != i+1)
                {
                    exit(EXIT_FAILURE);
                }
                return ret;
            }
        }
        if (nread > nleft)
        {
            exit(EXIT_FAILURE);
        }
        nleft -= nread;
        ret = readn(sockfd, bufp, nread);
        if (ret != nread)
        {
            exit(EXIT_FAILURE);
        }
        bufp += nread;
    }
    return -1;
}


int main(int argc, char** argv) {
    signal(SIGPIPE, SIG_IGN);

    // 1. 创建套接字
    int listenfd;
    if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        ERR_EXIT("socket");
    }

    // 2. 分配套接字地址
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // inet_aton("127.0.0.1", &servaddr.sin_addr);

    int on = 1;
    // 确保time_wait状态下同一端口仍可使用
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on) < 0)
    {
        ERR_EXIT("setsockopt");
    }

    // 3. 绑定套接字地址
    if (bind(listenfd, (struct sockaddr*) &servaddr, sizeof servaddr) < 0) {
        ERR_EXIT("bind");
    }
    // 4. 等待连接请求状态
    if (listen(listenfd, SOMAXCONN) < 0) {
        ERR_EXIT("listen");
    }
    
    struct pollfd client[MAXFDSIZE]; // 保存已连接描述符
    int maxi = 0;
    for (int i = 0; i < MAXFDSIZE; ++i) client[i].fd = -1;
    int nready;
    client[0].fd = listenfd;
    client[0].events = POLLIN;

    // 5. 允许连接
    struct sockaddr_in peeraddr;
    socklen_t peerlen = sizeof peeraddr;
    int connfd;


    while (1)
    {
        nready = poll(client, maxi+1, -1);

        // printf("nready = %d\n", nready);

        if (nready == -1) {
            if (errno == EINTR) continue;
            ERR_EXIT("poll");
        }

        if (nready == 0) continue; // 没有可读事件发生，继续轮询

        if (client[0].revents & POLLIN) { // 监听事件发生

            // printf("服务器端：Connections comes!\n");
            if ((connfd = accept(listenfd, (struct sockaddr *) &peeraddr, &peerlen)) < 0) {
                ERR_EXIT("accept");
            }
            printf("id = %s, ", inet_ntoa(peeraddr.sin_addr));
            printf("port = %d\n", ntohs(peeraddr.sin_port));
            printf("connfd = %d\n", connfd);

            int i = 0;
            for (; i < MAXFDSIZE; ++i) {
                if (client[i].fd == -1) {
                    client[i].fd = connfd;
                    maxi = max(maxi, i);
                    break;
                }
            }

            // printf("maxi = %d\n", maxi);
            if (i == MAXFDSIZE) {
                fprintf(stderr, "too many clients");
                exit(EXIT_FAILURE); 
            }
            
            client[i].events = POLLIN;
            if (--nready <= 0) continue;

        }

        for (int i = 1; i <= maxi; ++i) { // 从已连接套接字开始读，而不是监听套接字
            connfd = client[i].fd;
            if (connfd == -1) continue;

            if (client[i].revents & POLLIN) { // 发生可读事件
                char recvbuf[1024] = {0};
                // printf("服务器端：阻塞在readline\n");
                int ret = readline(connfd, recvbuf, 1024);
                // printf("服务器端：没有阻塞在readline\n");
                if (ret == -1)
                {
                    ERR_EXIT("readline");
                }
                if (ret == 0)
                {
                    printf("client close\n");
                    client[i].fd = -1;
                    close(connfd);
                }

                fputs(recvbuf, stdout);
                writen(connfd, recvbuf, strlen(recvbuf));

                if (--nready <= 0) break;
            }


        }


    }
    

    // 7. 断开连接
    close(listenfd);



    return 0;
}