### Echo server v1-v4中存在的问题

- Echo server v1-v4使用多进程实现来实现并发连接，频繁的进程切换会带来较大的开销

- 当与客户端连接的服务器端进程被杀死后，客户端会进入`FIN_WAIT2`状态，服务器端进入`CLOSE_WAIT`状态，原因是`fgets`一直在等待控制台输入，无法执行`read`操作，从而关闭客户端已连接套接字

### 如何解决？使用I/O多路复用

- 只用一个进程，进行事件监听（用select中的信号进行通信），一旦有事件发生，便进行读取

- 通过I/O复用实现


### 问题：

- `189`行中`rset = allset;`，为什么用`allset`赋值给`rset`?
  > 内核在遍历这个读集合的过程中，如果被检测的文件描述符对应的读缓冲区中没有数据，内核将修改这个文件描述符在读集合`fd_set`中对应的标志位，改为`0`，如果有数据那么这个标志位的值不变，还是`1`。这会导致`rset`改变，因此需要一个临时变量来记录所有的检测描述符。
  >
  > REF: https://subingwen.cn/linux/select/#2-2-%E7%BB%86%E8%8A%82%E6%8F%8F%E8%BF%B0


- select存在什么问题：
  1. 待检测集合（第 2、3、4 个参数）需要频繁的在用户区和内核区之间进行数据的拷贝，效率低
  2. 内核对于select传递进来的待检测集合的检测方式是线性的（虽然可以通过维护一个临时变量来减少遍历次数）
  3. 使用select能够检测的最大文件描述符个数有上限，默认是1024，如果需要改变，则需要重新编译内核