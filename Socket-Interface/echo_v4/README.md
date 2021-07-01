在echo server v3中提到：
> 在客户端代码中：
>  ```
>  // 等待控制台输入
>    while (fgets(sendbuf, sizeof sendbuf, stdin) != NULL) {
>        // write(clientfd, sendbuf, strlen(sendbuf)); // 将sendbuf写入缓冲区
>        write(clientfd, sendbuf, sizeof(sendbuf)); // 将sizeof sendbuf写入缓冲区
>        read(clientfd, recvbuf, sizeof recvbuf);
>        fputs(recvbuf, stdout); // 打印接受到的信息
>
>        memset(recvbuf, 0, sizeof recvbuf);
>        memset(sendbuf, 0, sizeof sendbuf);
>
>    }
>  ```
>  当与客户端连接的服务器端进程被杀死后，客户端会进入`FIN_WAIT2`状态，服务器端进入`CLOSE_WAIT`状态，原因是`fgets`一直在等待控制台输入，无法执行
> `read`操作（关闭客户端已连接套接字），造成这个的根本原因是`fgets`与`read`并发进行

### 解决办法：使用异步I/O

本节重点讲select，可以讲select视为一个管理者。

- 用select来管理多个I/O，一旦其中的一个I/O或者多个I/O检测到我们感兴趣的事件，select函数返回，返回值为检测到的事件个数
- 并且返回哪些I/O发生了事件，遍历这些事件，进而处理这些事件