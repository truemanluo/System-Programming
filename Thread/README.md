### Notes

- 为什么消费者线程中要用`while (nready == 0)`而不是`if (nready == 0)`?
  
  > 因为可能存在虚假唤醒（![Spurious wakeup](https://en.wikipedia.org/wiki/Spurious_wakeup)）。`pthread_cond_wait`实际上是执行三个操作：
  > - 对`mutex`解锁
  > - 等待条件，直到有线程向它发起通知
  > - 对`mutex`重新加锁

  > 因此，在等待条件到重新加锁之前，临界区属于可访问状态。假设条件满足，但是在重新加锁之前`nready`可能被其他线程改为`0`（不满足条件）。如果用`if`条件进行判断的话会造成虚假唤醒。
