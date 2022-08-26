#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "CallBack.h"
#include "Channel.h"
#include "FeiSocketops.h"
#include "InetAddress.h"
#include "EventLoopPool.h"
#include "Logging.h"
#include "tools.h"
#include <arpa/inet.h>
#include <set>
namespace feipu {
class EventLoop;

/*用于接受连接*/
class Acceptor {
public:
  typedef std::function<void(int, InetAddress)> NewConnectionCallback;
  Acceptor(EventLoop *loop, InetAddress listenAddr)
      : sockfd_(socket::createNonBlockingOrDie()),
        channel_(new Channel(sockfd_, loop)) {
    socket::bindOrDie(sockfd_, listenAddr.getRawSockAddr());
    LOG_INFO << "has bind";
  }
  void start() {
    socket::listenOrDie(sockfd_);
    channel_->setReadCall(
        std::bind(&Acceptor::handleNewConnect, this)); // 有新连接进入时
    channel_->enableRead();
  }
  void setNewConnectionCallback(NewConnectionCallback in_cb) { cb_ = in_cb; }

private:
  void handleNewConnect() {
    struct sockaddr_in remote_address_raw;
    int remotefd = socket::accept(sockfd_, &remote_address_raw);
    if (remotefd > 0) {
      if (cb_) {
        cb_(remotefd, InetAddress(remote_address_raw));
      } else {
        socket::closeOrDie(remotefd);
      }
    }
  }
  int sockfd_;
  Channel *channel_;
  NewConnectionCallback cb_;
};
// 单线程的tcpServer
class TcpServer : noncopyable {
public:
  TcpServer(EventLoop *loop, InetAddress listenAddr, string name);
  void start();
  void setThreadNum(unsigned short num);
  void setConnectionCallback(const ConnectionCallback &cb) { conn_cb_ = cb; }
  void setMessageCallback(const MessageCallback &cb) { message_cb_ = cb; }
  void setWriteAllCallback(const WriteAllCallback &cb) { write_cb_ = cb; }

private:
  void whenNewConnection(int remoteFd, InetAddress remoteAddr);
  void whenOldConnDisconnect(TcpConnectionPtr conn);
  void removeConnection(TcpConnectionPtr conn);
  EventLoop *loop_;
  InetAddress addr_;
  string name_;
  int nextConnId_;
  Acceptor acceptor_;
  std::set<TcpConnectionPtr> connections_; // 对connection有拥有权
  // FIXME 这里使用TcpConnectionPtr 作为key是否会导致不唯一?
  ConnectionCallback conn_cb_;
  MessageCallback message_cb_;
  WriteAllCallback write_cb_;

  EventLoopPool threads_pool_;
};
} // namespace feipu
#endif