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
/*
  -------TcpServer's thread_main----|______thread1(tcpconnections' loop)
                                    |
                                    |______thread2(tcpconnections' loop)
                                    |
                                    |______thread3(tcpconnections' loop)

        ConnectionCallback: accept时threadmain发出，断开由子线程发出
        MessageCallback: 总是由子线程发出
        WriteAllCallback: 总是由子线程发出

        ====>
  从而send调用必须考虑多线程，而buffer的读取由于其总是在子线程中无需考虑多线程。
*/
class TcpConnection : noncopyable,
                      public std::enable_shared_from_this<TcpConnection> {
public:
  TcpConnection(EventLoop *loop, int connfd, InetAddress localaddr,
                InetAddress peeraddr);
  ~TcpConnection();
  void setMessageCallback(const MessageCallback &cb) { message_cb_ = cb; }
  void setConnectionCallback(const ConnectionCallback &cb) { conn_cb_ = cb; }
  void setCloseCallback(const CloseCallback &cb) { close_cb_ = cb; }
  void setWriteCallback(const WriteAllCallback &cb) { write_cb_ = cb; }
  void send(const char *data, size_t len); // 线程安全的。
  void send(const string &data);           //线程安全的
  EventLoop *getloop() const { return loop_; }
  const InetAddress &localAddress() { return localaddr_; }
  const InetAddress &peerAddress() { return peeraddr_; }
  bool isConnected() { return status_ == ConnStatus::Connected; }
  void shutdown(); // 优雅地关闭连接,线程安全的

  /*给TcpServer使用*/
  void connectEstablished();

private:
  enum ConnStatus {
    Connected,
    Disconnected,
    Disconnecting, // 主动断开则转为这个状态。
  };
  void sendInLoop(const char *data, size_t len);
  // void sendInLoop(const string &data);
  void NetIntoBuffer(); // 将网络中的数据读入buffer中
  void BufferIntoNet(); // 将buffer的数据写入到网络之中
  void handleClose();   // 当获知连接关闭时
  void shutInLoop(); //主动关闭连接，但是只是关闭服务器的写端
  int fd_;
  InetAddress localaddr_;
  InetAddress peeraddr_;
  EventLoop *loop_;
  MessageCallback message_cb_;
  ConnectionCallback conn_cb_;
  CloseCallback close_cb_;
  WriteAllCallback write_cb_;
  std::unique_ptr<Channel> channel_;
  Buffer inBuffer_;
  Buffer outBuffer_;
  ConnStatus status_;
};
}; // namespace feipu
#endif