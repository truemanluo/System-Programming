#include "sysutils.h"

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
        // printf("客户端recv_peek\n");
        int ret = recv(sockfd, buf, len, MSG_PEEK); // 查看传入消息
        // printf("客户端recv_peek：ret = %d\n", ret);

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
        // printf("客户端recv\n");
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

void echo_cli(int sockfd) {
    char recvbuf[1024];
    char sendbuf[1024];
    // struct packet recvbuf;
    // struct packet sendbuf;
    memset(recvbuf, 0, sizeof recvbuf);
    memset(sendbuf, 0, sizeof sendbuf);
    while (fgets(sendbuf, sizeof sendbuf, stdin) != NULL)   // 键盘输入获取
    {
        writen(sockfd, sendbuf, strlen(sendbuf)); // 写入服务器

        // printf("客户端：写入服务器！\n");

        int ret = readline(sockfd, recvbuf, sizeof recvbuf);    // 服务器读取

        // printf("客户端：读取成功！ret = %d\n", ret);

        if (ret == -1)
        {
            ERR_EXIT("readline");
        }
        if (ret == 0)
        {
            printf("server close\n");
            break;
        }

        fputs(recvbuf, stdout); // 服务器返回数据输出

        // 清空
        memset(recvbuf, 0, sizeof recvbuf);
        memset(sendbuf, 0, sizeof sendbuf);
    }

    // 5. 断开连接
    close(sockfd);
}

void echo_cli_select(int sockfd) {
    fd_set rset;
    FD_ZERO(&rset);
    int maxfd; // 读集合的最大描述符

    char recvbuf[1024];
    char sendbuf[1024];
    // struct packet recvbuf;
    // struct packet sendbuf;
    memset(recvbuf, 0, sizeof recvbuf);
    memset(sendbuf, 0, sizeof sendbuf);

    // 检测事件
    int nready; // 已发生事件
    int fd_stdin = fileno(stdin);
    bool is_valid_stdin = true;

    maxfd = max(fd_stdin, sockfd);
    while (1) {
        if (is_valid_stdin)
            FD_SET(fd_stdin, &rset); // 将fd_stdin放入读描述符集合中
        FD_SET(sockfd, &rset);
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);
        if (nready == -1) {
            ERR_EXIT("select");
        }
        if (nready == 0) continue; // 超时
        if (FD_ISSET(sockfd, &rset)) {
            // 套接口产生可读事件
            int ret = readline(sockfd, recvbuf, sizeof recvbuf);    // 服务器读取
            if (ret == -1)
            {
                ERR_EXIT("readline");
            }
            else if (ret == 0)
            {
                printf("server close\n");
                break;
            }

            fputs(recvbuf, stdout); // 服务器返回数据输出

            // 清空
            memset(recvbuf, 0, sizeof recvbuf);
        }
        
        if (FD_ISSET(fd_stdin, &rset)) {
            // 标准输入产生可读事件
            if (fgets(sendbuf, sizeof sendbuf, stdin) == NULL) { 
                // close(sockfd); // 关闭描述符不退出会select会出现“Bad file descriptor”，也就是检测事件的描述符被关闭
                // exit(EXIT_SUCCESS);
                is_valid_stdin = false;
                shutdown(sockfd, SHUT_WR); // 可以保证发送出去的数据被echo回来

            }
            else {
                writen(sockfd, sendbuf, strlen(sendbuf)); // 写入服务器
                memset(sendbuf, 0, sizeof sendbuf);
            }
            

        }
    }
    close(sockfd);


}


int main(int argc, char** argv) {
    // 1. 创建套接字
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        ERR_EXIT("socket");
    }

    // 2. 分配套接字地址
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    // servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // inet_aton("127.0.0.1", &servaddr.sin_addr);

    // 3. 请求链接
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof servaddr) < 0) {
        ERR_EXIT("connect");
    }

    struct sockaddr_in localaddr;
    socklen_t addrlen = sizeof localaddr;
    if (getsockname(sockfd, (struct sockaddr*)&localaddr, &addrlen) < 0)
    {
        ERR_EXIT("getsockname");
    }
    printf("id = %s, ", inet_ntoa(localaddr.sin_addr));
    printf("port = %d\n", ntohs(localaddr.sin_port));
    printf("sockfd = %d\n", sockfd);

    // 4. 数据交换
    echo_cli(sockfd);


    return 0;
}