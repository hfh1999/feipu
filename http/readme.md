## http 简介
http模块参考了lib-hv和axum，意在达成简单好用的web业务编写

## http基本用法
http模块的核心类是HttpServer 和 Router
支持两种用法:
1. Router + handle
```
HttpServer server;
Router my_router;
my_router.route("/api1",GET,deal_api1) // deal_api1 为处理函数
         .route("/api2",GET,deal_api2);
server.serve(&my_router);
server.run();
```

2. Router + Service
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