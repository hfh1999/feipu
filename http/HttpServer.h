#ifndef httpserver_h
#define httpserver_h
#include "CallBack.h"
#include "FeiTypes.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include "tools.h"
namespace feipu {
class TcpServer;
class EventLoop;
class Router;
class HttpMsg;
class HttpHandler;
class Router;
class HttpServer : noncopyable {
public:
  HttpServer(EventLoop *loop, InetAddress listenAddr, string name);
  ~HttpServer();
  void start();
  void setThreadNum(unsigned short num); // 设定线程数
  void serve(Router *in_app); // 注册router

private:
  /* 绑定到tcpserver的read
      解析完整的信息后，产生内部表示，然后调用Route
  */
  void on_recv(TcpConnectionPtr, Buffer *);
  void on_connect(TcpConnectionPtr);
  void loop_on_connect(TcpConnectionPtr);

  void write_over();          // 发送完毕

  std::unique_ptr<TcpServer> tcpserver_;
  // 每个tcp连接对应一个http_handler_;
  std::map<TcpConnectionPtr,HttpHandler*> handler_map_; // 用智能指针代替之
  MutexLock mutex_;
  Router *http_router_;
  EventLoop* loop_;
};
} // namespace feipu
#endif