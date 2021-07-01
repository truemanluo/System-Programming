- 为什么需要`rio_readn`?
  - 字节流套接字上的read和write函数所表现的行为不同于通常的文件
IO。字节流套接字上调用read或write输入或输出的字节数可能比请求的数量少，然而这不是出错的状态。这个现象的原因在于内核中用于套接字的缓冲区可能已经到达了极限。此时所需的是调用者再次调用read或write函数，以输入或输出剩余的字节

  - 与`read`函数相比，它被信号处理函数中断后会再次尝试读取。因此，在除了可读字符数小于n情况下，该函数可以保证读取n个字节。
  > On files that support seeking, the read operation commences at
       the file offset, and the file offset is incremented by the number
       of bytes read.  If the file offset is at or past the end of file,
       no bytes are read, and read() returns zero.

  - 它可以接收定长的字节数，是TCP粘包问题的一种解决方案


- 为什么需要将`server`中：

  `write(clientfd, sendbuf, strlen(sendbuf));` 改为

  `write(clientfd, sendbuf, sizeof(sendbuf));`



存在的问题：

- 客户端关闭时，服务器捕获到消息，但是客户端会出现僵尸进程。处理办法:
  - 忽略SIGCHLD信号
  - 捕获信号，手动回收子进程

- 在客户端代码中：
  ```
  // 等待控制台输入
    while (fgets(sendbuf, sizeof sendbuf, stdin) != NULL) {
        // write(clientfd, sendbuf, strlen(sendbuf)); // 将sendbuf写入缓冲区
        write(clientfd, sendbuf, sizeof(sendbuf)); // 将sizeof sendbuf写入缓冲区
        read(clientfd, recvbuf, sizeof recvbuf);
        fputs(recvbuf, stdout); // 打印接受到的信息

        memset(recvbuf, 0, sizeof recvbuf);
        memset(sendbuf, 0, sizeof sendbuf);

    }
  ```
  当与客户端连接的服务器端进程被杀死后，客户端会进入`FIN_WAIT2`状态，服务器端进入`CLOSE_WAIT`状态，原因是`fgets`一直在等待控制台输入，无法执行`read`操作（关闭客户端已连接套接字），造成这个的根本原因是`fgets`与`read`并发进行