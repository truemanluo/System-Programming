### 套接字I/O超时检测

### POLL实现并发
- select的限制：
  - 一个进程能够打开的最大文件描述符限制，可以调整调整内核参数（但是只针对当前进程）
  - select中fd_set集合容量的限制(`FD_SETSIZE`)，这需要重新编译内核

### What's new

- 重构代码，分离出head files
- 实现套接字I/O超时检测函数
  - `read_timeout`
  - `write_timeout`
  - `accept_timeout`
  - `connect_timeout`

### Notes
- 为什么开启多个client时，文件描述符都从未使用的最小描述符开始分配（本例中都为3）？
  > 因为每个进程都有自己的独立空间，而文件描述符只是对文件的引用，不同进程的文件描述符它们互不影响

- Flags设置中的操作
  > https://stackoverflow.com/questions/22008229/bitwise-or-in-linux-open-flags

- 为什么需要调用`activate_noblock`和`deactivate_noblock`？
  > 因为是connect是阻塞的，如果直接调用会导致超时检测阻塞，这不是我们所期望的

- 为什么`getsockopt`中`OPTLEN`是指向`socklen_t`的指针（`accept`函数也是类似）?
  > it's to be able to write the length back. It's an input/output parameter: "The addrlen argument is a value-result argument: the caller must initialize it to contain the size (in bytes) of the structure pointed to by addr; on return it will contain the actual size of the peer address." 

- Bug: 从监听套接字读取数组，造成读取异常，程序退出

`for (int i = 1; i <= maxi; ++i) // 从已连接套接字开始读，而不是监听套接字`


### Bugs
- [ ] Unfix:      
  - 服务器端226行`if (client[i].revents & POLLIN) // 发生可读事件`改为`if (client[i].events & POLLIN)`时只能一个客户端进行数据传输，具体还不知道问题阻塞在哪（可能是某个I/O函数）