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

## 进度
1. 基本完成http的解析部分.
2. 基本完成router的匹配部分.
3. 完成整个模块的编写，待测试.

## TODO
1. 支持Keep-alive
2. 增加连接定时器，在超时时主动中断连接
3. 支持大文件
4. 编写service设施

## 疑问
浏览器发起一次请求的时候，竟然会同时打开两个Tcp连接，是bug还是特性?