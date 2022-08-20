# FeiPu

一个简陋的muduo仿制品，用于学习之用

### 特性说明
1. 关于定时器:没有像muduo一样使用timefd来管理定时器，而采取了传统的定时器管理方式(类似libevent).
2. 关于线程，简单地包装了c++ 11自带的thread类和lock_guard,condition类等.
3. 关于日志，目前只完成了一个简单的日志前端，只能在单线程环境下工作，且效率很差.
4. 已经完成了一个多线程的TcpServer，主要思想是利用One loop per Thread 和 线程池.

5. 以TcpServer为基础完成了HttpServer


### http模块原理
主要思想借鉴了以下几个项目:

[TinyWebServer](https://github.com/qinguoyi/TinyWebServer)

[lib-hv](https://github.com/ithewei/libhv)

[axum](https://github.com/tokio-rs/axum)

```
HTTP请求 -→ 解析 -→ route -→  |内部表示 in_data|
                                        |
                                        |(Http Service/Handle)
                                        |
                                        ↓
HTTP回复 ←- 序列化 ←--------- |内部表示out_data|
```
## http 简介
http模块参考了lib-hv和axum，意在达成简单好用的web业务编写

## http基本用法
http模块的核心类是HttpServer 和 Router
支持两种用法:
1. Router + handle

下面的代码在8989端口开启web服务器.

当浏览器访问http://your_server_ip:8989:index.html 后呈现"Hello HTML5"字样
```
    EventLoop loop;
    HttpServer myweb(&loop,InetAddress(8989),"test_web");
    Router my_service;
    my_service.route("/index.html",HttpMethod::HTTP_GET,[](const HttpRequest* req,HttpResponse * resp){
        resp->setContent(string("<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Hello World</title></head><body><p>Hello HTML5</p></body></html>"));
    });
    myweb.serve(&my_service);
    myweb.start();
    loop.loop();
```

2. Router + Service(还未实现)

可将常见的业务打包进Service,只要继承并完善Service接口即可.
```
HttpServer server;
FileService file_service; //继承自Service接口,用作返回静态文件
Router my_router;
my_router.route("/index.html",&file_service) // 注册服务
         .route("/api2",GET,deal_api2);
server.serve(&my_router);
server.run();
```

## 匹配与捕捉

通过route将handler和Service联系起来，route根据URL和方法进行匹配请求到相应的逻辑中。

"*res" 表示匹配URL之后的所有字段到HttpRequest中的path_match key-value结构的"res"字段中

":param" 表示匹配URL中的对应位置的字段到HttpRequest中的path_match key-value结构的"param"字段中

```
HttpServer server;
Router my_router;
my_router.route("/api1/:param",GET,deal_api1) // ":"为字段匹配
         .route("/api2/*res",GET,deal_api2); //  "*"匹配剩下所有
         .route("/api3/:param/*res") // 可以混合使用
server.serve(&my_router);
server.run();
```

### TODO(按优先级排列)
1. 编写HTTP模块             <== working
2. 编写多线程的UdpServer
3. 给库提供处理信号的能力
4. 目前使用红黑二叉树(std::set)来管理定时器,但计划采用优先级队列(堆)替换之
5. 计划编写多线程日志，提升效率
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