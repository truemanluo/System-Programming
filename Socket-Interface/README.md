- 套接字地址结构
  
- 字节序
    - 网络字节序（大端）
    - 主机字节序

    ```#include <arpa/inet.h>```中提供了不同字节序的转换函数

- 套接字类型
  - 流式套接字
  - 数据报式套接字
  - 原始套接字

- 服务器端`listen`时设置的`SOMAXCONN`是什么
  > TCP 建立连接时要经过 3 次握手，在客户端向服务器发起连接时，
  对于服务器而言，一个完整的连接建立过程，服务器会经历 2 种 TCP 状态：SYN_REVD, ESTABELLISHED。对应也会维护两个队列：
    - 一个存放SYN的队列（半连接队列，也成SYN队列）
    - 一个存放已经完成连接的队列（全连接队列， 也称Accept队列）
  
  > 当一个连接的状态是 SYN RECEIVED 时，它会被放在 SYN 队列中。
  当它的状态变为 ESTABLISHED 时，它会被转移到另一个队列。应用程序只从已完成的连接的队列中获取请求。

  ![](https://gblobscdn.gitbook.com/assets%2F-MRP-_mTHVixnnXPCTr3%2F-Md0sntwCDKWfjej7AI_%2F-Md5H0CSgkI24kPyiDUN%2Fimage.png?alt=media&token=4c907726-2144-40df-9bea-ef0bc28b5d8e)