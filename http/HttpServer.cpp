#include "HttpServer.h"
#include "Buffer.h"
#include "HttpHandler.h"
#include "HttpMsg.h"
#include "Logging.h"
#include "TcpServer.h"
namespace feipu {
HttpServer::HttpServer(EventLoop *loop, InetAddress listenAddr, string name)
    : tcpserver_(new TcpServer(loop, listenAddr, name)), http_router_(nullptr),
      loop_(loop) {}
HttpServer::~HttpServer() {}
void HttpServer::start() {
  tcpserver_->setConnectionCallback(
      std::bind(&HttpServer::on_connect, this, _1));
  tcpserver_->setMessageCallback(std::bind(&HttpServer::on_recv, this, _1, _2));
  tcpserver_->start();
}
void HttpServer::setThreadNum(unsigned short num) {
  tcpserver_->setThreadNum(num);
}
void HttpServer::on_recv(TcpConnectionPtr conn, Buffer *buffer) {
  LOG_TRACE << "conn = [" << conn->getName() << "]"
            << " recieved.";
  mutex_.lock();
  assert(handler_map_.find(conn) != handler_map_.end());
  HttpHandler *handler = handler_map_[conn];
  mutex_.unlock();
  HttpHandler::ParseStatus parse_status = handler->parse_http(buffer);
  if (parse_status == HttpHandler::PARSE_ERROR) {
    // 解析错误,中断连接
    conn->shutdown(); // FIXME 是否需要强制中断?
  } else if (parse_status == HttpHandler::PARSE_RECV) {
    // 需要更多的数据来解析
    return;
  } else if (parse_status == HttpHandler::PARSE_NEXT) {
    LOG_FATAL << "Parse http : INNER ERROR";
  }

  //LOG_INFO << handler->ret_request()->Dump(true, true);
  // PARSE_END,解析完成,进行处理
  LOG_INFO << "[HttpServer::on_recv] parse finish - header count = "
           << handler->ret_request()->headers.size() << "- Web Req's conn is "
           << conn->getName();
  handler->handle_http(conn);
  conn->shutdown();
}
void HttpServer::on_connect(TcpConnectionPtr conn) {
  LOG_INFO << "connected status = " << conn->isConnected()
           << " thread id = " << conn->getloop()->getTid() << " HttpServer id = " << loop_->getTid();
  if (conn->isConnected()) {
    mutex_.lock();
    if (handler_map_.find(conn) == handler_map_.end()) {
      handler_map_[conn] =
          new HttpHandler; // FIXME 是否会引起conn增加引用计数？
      handler_map_[conn]->init(http_router_);
      //for (auto item : handler_map_) {
      //  LOG_INFO << "handler_map_ : " << item.first->getName();
      //}
    } else {
      LOG_TRACE << "conn is existing.";
    }
    mutex_.unlock();
  } else {
    mutex_.lock();
    //for (auto item : handler_map_) {
    //  LOG_INFO << "handler_map_ : " << item.first->getName();
    //}
    if (handler_map_.find(conn) != handler_map_.end()) {
      auto to_delete = handler_map_[conn];
      delete (to_delete);
      handler_map_.erase(conn);
    } else {
      LOG_FATAL << "handler's conn not exist!";
    }
    mutex_.unlock();
  }
  //loop_->runInLoop(std::bind(&HttpServer::loop_on_connect, this, conn));
  //loop_on_connect(conn);
}
void HttpServer::loop_on_connect(TcpConnectionPtr conn) {
  loop_->assertInLoopThread();
  if (conn->isConnected()) {
    LOG_INFO << "[HttpServer::new Connection] " << conn->getName();
    if (handler_map_.find(conn) == handler_map_.end()) {
      LOG_INFO << "[HttpServer] new in map.";
      handler_map_[conn] =
          new HttpHandler; // FIXME 是否会引起conn增加引用计数？
      handler_map_[conn]->init(http_router_);
      for (auto item : handler_map_) {
        LOG_INFO << "handler_map_ : " << item.first->getName();
      }
    } else {
      LOG_INFO << "conn is existing.";
    }
  } else {
    LOG_INFO << "[HttpServer: disconnect]" << conn->getName();
    for (auto item : handler_map_) {
      LOG_INFO << "handler_map_ : " << item.first->getName();
    }
    if (handler_map_.find(conn) != handler_map_.end()) {
      auto to_delete = handler_map_[conn];
      delete (to_delete);
      handler_map_.erase(conn);
    } else {
      LOG_FATAL << "handler's conn not exist!";
    }
  }
}
void HttpServer::serve(Router *in_app) { http_router_ = in_app; }
} // namespace feipu