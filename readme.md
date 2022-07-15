# FeiPu

一个简陋的muduo仿制品，用于学习之用

### 特性说明
1. 关于定时器:没有像muduo一样使用timefd来管理定时器，而采取了传统的定时器管理方式(类似libevent).
2. 关于线程，简单地包装了c++ 11自带的thread类和lock_guard,condition类等.
3. 关于日志，目前只完成了一个简单的日志前端，只能在单线程环境下工作，且效率很差.
4. 已经完成了一个多线程的TcpServer，主要思想是利用One loop per Thread 和 线程池.

### TODO(按优先级排列)
1. 编写HTTP模块             <== working
2. 编写多线程的UdpServer
3. 给库提供处理信号的能力
4. 目前使用红黑二叉树(std::set)来管理定时器,但计划采用优先级队列(堆)替换之
5. 计划编写多线程日志，提升效率

### http模块原理
主要思想借鉴了以下几个项目:

[TinyWebServer](https://github.com/qinguoyi/TinyWebServer)

[lib-hv](https://github.com/ithewei/libhv)

[axum](https://github.com/tokio-rs/axum)

```
HTTP请求 -→ 解析 -→ route -→  |内部表示 in_data|
                                        |
                                        |(Http Service Handle)
                                        |
                                        ↓
HTTP回复 ←- 序列化 ←--------- |内部表示out_data|
```

详细用法见:
[Http模块简介](http/readme.md)

### HOW TO USE
0. 编译准备.
```
    # on Unbuntu.
    $sh> sudo apt install g++ cmake make
```
1. 编译
```
    $sh> cmake
```
编译好的库文件是 ```${feipu路径}/build/libfeipu.a```

2. power你的项目

在你的项目的MakeLists的包含路径中增加 ```${feipu路径}/include```

最后链接 ```libfeipu.a```