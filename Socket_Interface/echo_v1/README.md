服务器端流程：
1. 用`getaddrinfo`做些初始化

    `getaddrinfo`函数将主机名、主机地址、服务名和端口号的字符串转化为套接字的地址结构，使得**能够编写独立于任何特定版本的IP协议的网络程序**。

    返回一个指向`addrinfo`的链表，其中每个结构指向对应host和service的套接字结构。

    <img src="https://gblobscdn.gitbook.com/assets%2F-MRP-_mTHVixnnXPCTr3%2F-Md0sntwCDKWfjej7AI_%2F-Md2FtFt4dYZ4IrD41h6%2Faddr.JPG?alt=media&token=ab8b9e7d-74f6-48e0-bc71-0551065c33f1" width="80%">


    函数签名：
    
    `int getaddrinfo(const char *node, const char *service,
                       const struct addrinfo *hints,
                       struct addrinfo **res);`
    
    `node`是连接的主机名或者IP地址，`service`指端口号或者服务（比如“http”）


2. 创建socket
   
   需要注意的是，创建socket的过程中需要处理几个异常：
   - 创建socket失败
   - 端口被占用
3. 绑定描述符
   
   服务器端有两个描述符：监听描述符（用于监听，等待客户端的连接请求）和连接描述符（用于数据传输），这样设计的目的是为了让服务器端能都同时处理多个连接，实现并发。
4. 监听
5. Accept
6. 读写数据

    客服端和服务器的数据传输是通过读写各自的已**连接文件描述符**实现的


客户端流程：

1. 创建socket
2. connect
3. 读写数据


Echo-Server-V1存在的问题：

- 无法实现多个客户端连接同一个服务器时发送数据的请求
  > 可以实现连接，但无法传输数据

- [X] 在`client`中没有清空缓冲区为什么没有出现问题?
  > 因为用了`sizeof sendbuf`，无论在控制台输入多少字符都会写1024个字符，将`sendbuf`。如果改成`strlen(sendbuf)`的话会出现缓冲区覆盖的情况
  
    ```
    while (fgets(sendbuf, sizeof sendbuf, stdin) != NULL) {
        write(clientfd, sendbuf, sizeof sendbuf); // 将sendbuf写入缓冲区
        read(clientfd, recvbuf, sizeof recvbuf);
        fputs(recvbuf, stdout); // 打印接受到的信息

        // 为什么此处没有清空缓冲区不存在Bug
    }
    ```
    **FIX**：
    每次打印后清空缓冲区

- 两次相隔时间较短的连接会出现`TIME_WAIT`现象