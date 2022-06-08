# FeiPu

一个简陋的muduo仿制品，用于学习之用

### 进度说明

1. 关于定时器:没有像muduo一样使用timefd来管理定时器，而采取了传统的定时器管理方式(类似libevent).目前使用红黑二叉树(std::set)来管理定时器,计划采用自己编写的heap来替换红黑树.

2. 关于线程，简单地包装了c++ 11自带的thread类和lock_guard,condition类等.

3. 关于日志，目前只完成了一个简单的日志前端，只能在单线程环境下工作，且效率很差。

### TODO
1. 完成一个简单的TcpServer和TcpClient.