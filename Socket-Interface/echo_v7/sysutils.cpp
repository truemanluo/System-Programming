#include "sysutils.h"
#define ERR_EXIT(m) \
        do  \
        {   \
            perror(m);  \
            exit(EXIT_FAILURE); \
        } while(0);

/**
 * 
 * @param  {int} fd       : 检测的描述符
 * @param  {unsigned int} : 超时时间
 * @return {int}          : 未超时返回0，否则返回-1
 */
int read_timeout(int fd, unsigned int wait_seconds) {
    int ret = 0;
    if (wait_seconds > 0) {
        fd_set rset;
        FD_ZERO(&rset);
        FD_SET(fd, &rset);

        struct timeval timeout;
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0; 

        // 读事件未发生或者被中断时轮询
        do {
            ret = select(fd+1, &rset, NULL, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);

        // 读事件发生或者返回-1
        if (ret > 0) {
            ret = 0;
        }
        else if (ret == 0) {
            // 超时
            ret = -1;
            errno = ETIMEDOUT; // 设置超时标志
        }
    }
    return ret;

}

/**
 * 
 * @param  {int} fd       : 检测的描述符
 * @param  {unsigned int} : 超时时间
 * @return {int}          : 未超时返回0，否则返回-1
 */
int write_timeout(int fd, unsigned int wait_seconds) {
    int ret = 0;
    if (wait_seconds > 0) {
        fd_set wset;
        FD_ZERO(&wset);
        FD_SET(fd, &wset);

        struct timeval timeout;
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0; 

        // 读事件未发生或者被中断时轮询
        do {
            ret = select(fd+1, NULL, &wset, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);

        // 读事件发生或者返回-1
        if (ret > 0) {
            ret = 0;
        }
        else if (ret == 0) {
            // 超时
            ret = -1;
            errno = ETIMEDOUT; // 设置超时标志
        }
    }
    return ret;

}

/**
 * 
 * @param  {int} fd              : 
 * @param  {struct*} sockaddr_in : 输出参数，返回对方地址
 * @param  {unsigned} int        : 等待超时的秒数，为0表示正常模式
 * @return {int}                 : 返回已连接套接字

 */
int accept_timeout(int fd, struct sockaddr_in *addr ,unsigned int wait_seconds) {
    int ret;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    if (wait_seconds > 0) {
        fd_set accept_fdset;
        FD_ZERO(&accept_fdset);
        FD_SET(fd, &accept_fdset);

        struct timeval timeout;
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0; 

        // 读事件未发生或者被中断时轮询
        do {
            ret = select(fd+1, &accept_fdset, NULL, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);

        // 读事件发生或者返回-1
        if (ret == -1) {
            return -1;
        }
        else if (ret == 0) {
            // 超时
            errno = ETIMEDOUT; // 设置超时标志
            return -1;
        }
    }
    if (addr != NULL) {
        ret = accept(fd, (struct sockaddr*)addr, &addrlen);
    }
    if (ret == -1) {
        ERR_EXIT("accept"); // 错误
    }
    return ret;
}

// 将I/O设为阻塞模式
void activate_noblock(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        ERR_EXIT("fcntl");
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        ERR_EXIT("fcntl");
    }
}

// 解除阻塞模式
void deactivate_noblock(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        ERR_EXIT("fcntl");
    }
    flags &= ~O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        ERR_EXIT("fcntl");
    }
}


/**
 * 
 * @param  {int} fd              : 
 * @param  {struct*} sockaddr_in : 输出参数，返回对方地址
 * @param  {unsigned} int        : 等待超时的秒数，为0表示正常模式
 * @return {int}                 : 返回已连接套接字

 */
int connect_timeout(int fd, struct sockaddr_in *addr ,unsigned int wait_seconds) {
    int ret;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    if (wait_seconds > 0) {
        activate_noblock(fd);
    }
    // 如果wait_seconds == 0，则不检测超时，以阻塞方式调用
    ret = connect(fd, (struct sockaddr*)addr, addrlen);
    if (ret < 0 && errno == EINPROGRESS) { // 表示正在连接，检测超时
        fd_set connect_fdset;
        FD_ZERO(&connect_fdset);
        FD_SET(fd, &connect_fdset);

        struct timeval timeout;
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0; 

        // 读事件未发生或者被中断时轮询
        do {
            ret = select(fd+1, NULL, &connect_fdset, NULL, &timeout);
        } while (ret < 0 && errno == EINTR);

        // 读事件发生或者返回-1
        if (ret == -1) {
            return -1;
        }
        else if (ret == 0) {
            // 超时
            errno = ETIMEDOUT; // 设置超时标志
            return -1;
        }
        else if (ret == 1) { // 套接字出错也会出现可写事件
            /**
             * ret返回1可能有两种情况，一种是连接建立成功，一种是套接字产生错误。
             * 此时错误信息不会保存在errno中，因此需要调用getsockopt来获取
            **/
           int err;
           socklen_t socklen = sizeof(err);
           int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
           if (sockoptret == -1) {
               return -1; // 出错
           }
           if (err == 0) {
               ret = 0;
           }
           else {
               errno = err; // 否则设置错误码
               return -1;
           }
        }
    }
    if (wait_seconds > 0) {
        deactivate_noblock(fd);
    }
    return ret;
}