#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H
#include "Buffer.h"
#include "CallBack.h"
#include "Channel.h"
#include "EventLoop.h"
#include "FeiTypes.h"
#include "InetAddress.h"
#include "tools.h"
namespace feipu {
/*本类应该具有自动化的生命周期管理*/
/*负责管理缓冲区*/
class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection> {
public:
  TcpConnection(Eventloop *loop, int connfd, InetAddress connAddr);
  ~TcpConnection();
  void setMessageCallback(const MessageCallback &cb) { message_cb_ = cb; }
  void setConnectionCallback(const ConnectionCallback& cb){conn_cb_ = cb;}
  void setCloseCallback(const CloseCallback& cb){close_cb_ = cb;}
  void setWriteCallback(const WriteAllCallback& cb){write_cb_ = cb;}
  void send(const char* data,size_t len);
  void connectEstablished();

private:
  void NetIntoBuffer(); // 将网络中的数据读入buffer中
  void BufferIntoNet(); // 将buffer的数据写入到网络之中
  int fd_;
  InetAddress addr_;
  Eventloop *loop_;
  MessageCallback message_cb_;
  ConnectionCallback conn_cb_;
  CloseCallback close_cb_;
  WriteAllCallback write_cb_;
  std::unique_ptr<Channel> channel_;
  Buffer inBuffer_;
  Buffer outBuffer_;
};
}; // namespace feipu
#endif