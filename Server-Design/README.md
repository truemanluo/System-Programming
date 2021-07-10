## 大并发服务器设计

- 基本思路：https://www.cnblogs.com/secondtonone1/p/7076769.html

### I/O多路复用优化

- `read`/`write`流程优化

![](https://gblobscdn.gitbook.com/assets%2F-MRP-_mTHVixnnXPCTr3%2F-MeDi5PnlLBQlHQc9Db-%2F-MeDi85_AiqtBs5GRnbm%2Fimage.png?alt=media&token=611c22b8-a5c1-42b2-be67-afe206e203e6)

  - `POLLOUT`事件：当内核缓冲区不满时发生

- `EMFILE`的处理

- EPOLLET和EPOLLLT模式下的逻辑
  - EPOLLET

  ![](https://gblobscdn.gitbook.com/assets%2F-MRP-_mTHVixnnXPCTr3%2F-MeDi5PnlLBQlHQc9Db-%2F-MeEU9djZOYii-t-bo2U%2Fimage.png?alt=media&token=2d548274-e783-4d66-8d69-ee1a7e627967)
  - EPOLLLT

  ![](https://gblobscdn.gitbook.com/assets%2F-MRP-_mTHVixnnXPCTr3%2F-MeDi5PnlLBQlHQc9Db-%2F-MeEUIYln_dWNB7jOVxY%2Fimage.png?alt=media&token=eb536800-f022-4a3d-9c93-de8b29b250af)