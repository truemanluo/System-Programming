### 进程间通信的目的

- 数据传输
- 资源共享
- 通知事件
- 进程控制

### 进程间通信的手段
  - 文件（读写同一个文件）
  - 文件锁（读写锁，排他）
  - 管道（pipe）和FIFO
  - 信号

- POSIX IPC
  - 消息队列
  - 共享内存
    - 管道、消息队列与共享内存传递数据的对比
      - 共享内存只用两次系统调用
        ![](https://gblobscdn.gitbook.com/assets%2F-MRP-_mTHVixnnXPCTr3%2F-Mdqlen5hVsaczNs7g8x%2F-MdujdA20DeTXJfm5Vwi%2Fimage.png?alt=media&token=69e13bfd-2d6d-4c68-8621-a83b775b3b97)

      - 管道和消息队列需要四次系统调用

        ![](https://gblobscdn.gitbook.com/assets%2F-MRP-_mTHVixnnXPCTr3%2F-Mdqlen5hVsaczNs7g8x%2F-MdukBNQM7M-XVH-mLfQ%2Fimage.png?alt=media&token=a2c3d9a3-94e0-45fb-bf20-32de65ef4dfc)
  - 信号量
  - 互斥量（mutex）
  - 条件变量
  - 读写锁

#### IPC对象的持续性
- 随进程持续：一直存在直到打开的最后一个进程结束（如pipe和FIFO）
- 随内核持续：一直存在直到内核自举或显示删除（如System V消息队列、共享内存和信号量）
- 随文件持续：一直存在直到显示删除，即使内核自举还存在（如Posix消息队列、共享内存和信号量如果是使用映射文件来实现）

### Unix进程共享信息的三种方式

- 共享内存区不用进行内核态和用户态或进行文件访问，效率较高

![](https://gblobscdn.gitbook.com/assets%2F-MRP-_mTHVixnnXPCTr3%2F-Mdqlen5hVsaczNs7g8x%2F-MdqliK9CUOKm987AOiE%2Fimage.png?alt=media&token=ec72526e-6654-414d-b10a-5411dc7f16d3)

### 死锁
#### 死锁产生的必要条件
- 互斥条件
  
  进程对资源进行排他性使用
- 请求和保持条件

    当进程因请求资源而阻塞时，对已获得的资源保持不放
  
- 不可剥夺条件
- 环路等待条件
  
  各个进程形成环路的等待

#### 死锁解决办法


### Notes:
- 关于文件权限的问题：https://superuser.com/questions/295591/what-is-the-meaning-of-chmod-666


### Bugs:
- 一个难以察觉的错误
    - `mmap_test_read.c`中`fd = open(argv[1], O_RDONLY);`被以只读方式打开，但是对于共享内存的`prot`设置的是`PROT_READ | PROT_WRITE`（即可读可写），因此会出现Segmentation fault
  
