#include "HttpServer.h"
#include "Buffer.h"
#include "HttpHandler.h"
#include "HttpMsg.h"
#include "TcpServer.h"
#include "Logging.h"
namespace feipu {
HttpServer::HttpServer(EventLoop *loop, InetAddress listenAddr, string name)
    : tcpserver_(new TcpServer(loop, listenAddr, name)), http_router_(nullptr) {}
HttpServer::~HttpServer() {}
void HttpServer::start() {
  tcpserver_->setMessageCallback(std::bind(&HttpServer::on_recv, this, _1, _2));
}
void HttpServer::on_recv(TcpConnectionPtr conn, Buffer *buffer) {
  assert(handler_map_.find(conn) != handler_map_.end());
  HttpHandler* handler =  handler_map_[conn];
  HttpHandler::ParseStatus parse_status = handler->parse_http(buffer);
  if (parse_status == HttpHandler::PARSE_ERROR) {
    // 解析错误,中断连接
    conn->shutdown(); // FIXME 是否需要强制中断?
  } else if (parse_status == HttpHandler::PARSE_RECV) {
    // 需要更多的数据来解析
    return;
  }else if(parse_status == HttpHandler::PARSE_NEXT)
  {
    // 需要更多数据
    LOG_FATAL << "Parse http : INNER ERROR";
  }

  // END 解析完成

  // PARSE_END,解析完成,进行处理
  handler->handle_http();
}
void HttpServer::on_connect(TcpConnectionPtr conn)
{
  if(conn->isConnected())
  {
    if(handler_map_.find(conn) == handler_map_.end())
    {
      handler_map_[conn] = new HttpHandler; // FIXME 是否会引起conn增加引用计数？
      handler_map_[conn]->init(http_router_);
    }
    else
    {
      LOG_TRACE << "conn is existing.";
    }
  }
  else
  {
    if(handler_map_.find(conn) != handler_map_.end())
    {
      auto to_delete = handler_map_[conn];
      delete(to_delete);
      handler_map_.erase(conn);
    }
    else
    {
      LOG_FATAL << "handler's conn not exist!";
    }
  }
}
void HttpServer::serve(Router *in_app) { http_router_ = in_app; }
} // namespace feipu