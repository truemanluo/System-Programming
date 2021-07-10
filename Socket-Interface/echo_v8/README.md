### Epoll

#### 构建一个Epoll应用（服务器端）的流程及相关函数

1. 创建一个Epoll实例（返回文件描述符`epfd`）
  
    `int epoll_create(int sizes); // 底层是hash实现的`

    `int epoll_create1(int flags); // 底层由红黑树实现`


    > `epoll_create()`  creates a new epoll(7) instance. `epoll_create() `returns a file descriptor referring to the new epoll instance.  This file  descriptor  is  used
       for all the subsequent calls to the epoll interface.  When no longer required, the file descriptor returned by
       `epoll_create()` should be closed by using `close(2)`.  When all file descriptors referring to an  epoll  instance
       have been closed, the kernel destroys the instance and releases the associated resources for reuse.

2. 执行对Epoll实例的控制操作（监听套接字）
   
   `int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);`

   - 其中`struct epoll_event`描述链接到epoll实例的对象，定义如下：
    ```
        typedef union epoll_data {
               void        *ptr;
               int          fd;
               uint32_t     u32;
               uint64_t     u64;
        } epoll_data_t;

        struct epoll_event {
            uint32_t     events;      /* Epoll events */
            epoll_data_t data;        /* User data variable */
        };
    ```
    其中`epoll_data`是一个共用体，可以是一个指向包含复杂信息的结构体的指针，也可以是一个文件描述符。Epoll events指感兴趣的事件，比如`event.events = EPOLLIN | EPOLLET`表示关心可读事件，并以边缘方式触发

3. 定义一个`Eventlist`动态数组存放`events`, `clients`数组保存已连接套接字

4. `epoll_wait`：在一个epoll文件描述符上（`epfd`）等待I/O事件，返回发生的事件（`nready`）个数

    `int epoll_wait(int epfd, struct epoll_event *events,
                      int maxevents, int timeout);`
    `events`指针指向Eventlist

5. 检测事件：
   - 在监听描述符上是否发生事件
   - 在已连接描述符上是否发生事件
     - 发生事件：
       - 读到空，关闭已连接描述符，将`event`从`epfd`管理的套接字中移除，删除`clients`中的对应客户端



### Epoll与select、poll的区别

- 相较于`select`与`poll`而言，`epoll`的最大好处在于它不会随着监听`fd`数目的增长而降低效率

- 内核中的`select`与`poll`的实现是使用轮询来处理的，轮询的fd数目越多耗时越多

- `epoll`的实现是基于回调的，如果fd有期望的事件发生，就通过回调函数将其加入`epoll`就绪队列中，也就是它只关心"活跃"的fd，与fd数目无关

- 内核/用户空间内存拷贝的问题，事件发生后，内核如何将对应的fd和消息通知给应用程序呢？`epoll`是通过共享内存实现的，而`select`和`poll`是通过内存拷贝实现的

- `epoll`不仅会告诉应用程序有I/O事件到来，还会告诉应用程序相关的信息（`epoll_data`）。这些信息是由应用程序填充的，因此根据这些信息应用程序就能直接定位到事件，而不必遍历整个fd集合
  > 比如，在`select`应用中，`select`函数需要提供`maxfd`参数，因此需要`O(n)`时间来遍历fd集合检测是否有感兴趣的事件发生。同样在应用层还需要遍历存放已连接的文件描述符数组，看他们是否发生事件


### Epoll的两种触发模式
- 两种事件
    - `EPOLLIN`事件
      - 低电平：内核中的socket接收缓冲区为**空**
      - 高电平：内核中的socket接收缓冲区**不为空**
    - `EPOLLOUT`事件
      - 低电平：内核中的socket发送缓冲区**满**
      - 高电平：内核中的socket发送缓冲区**不满**

- EPOLLLT(epoll level triger)

    高电平触发。完全靠`kernel epoll`驱动，应用程序只需要处理从`epoll_wait`返回的fds，这些fds被认为是处于就绪状态

- EPOLLET(epoll edge triger)

    边沿触发条件：
    - 低电平-->高电平
    - 高电平-->低电平
  
    此模式下，系统仅仅通知应用程序哪些fds从空闲状态变成就绪状态，**一旦fd变成就绪状态，`epoll`将不再关注这个fd的任何状态信息**，（从`epoll`队列移除）直到应用程序通过读写操作触发`EAGAIN`状态，`epoll`认为这个fd又变为空闲状态，那么`epoll`又重新关注这个`fd`的状态变化（重新加入`epoll`队列）

> EPOLLET需要应用程序维护一个就绪队列（需要将读写置为非阻塞模式，以便能触发`EAGAIN`状态，重新将fd加入`epoll`队列），而EPOLLLT由内核维护。在EPOLLET模式下，队列中的fds是在减少的，所以在大并发的系统中，EPPLLET更有优势，但对程序员的要求更高