/*一个简单的单线程udpserver*/
#ifndef UDPSERVER_H
#define UDPSERVER_H
#include "Buffer.h"
#include "EventLoop.h"
#include "FeiTypes.h"
#include "InetAddress.h"
#include "tools.h"
#include <map>
#include <queue>
namespace feipu {
class UdpSocket;
class Channel;
struct Datagram {
  InetAddress addr_;
  string data_;
  bool is_valid() { return addr_.is_valid(); }
};

/*封装接口*/
// 线程不安全
class DatagramStream {
public:
  DatagramStream(std::queue<Datagram> &in_data) : in_data_ref(in_data) {}
  Datagram reciveDatagram() { // 需要检查返回的数据是否有效
    if (in_data_ref.empty()) {
      return Datagram();
    }
    Datagram ret = in_data_ref.front(); // FIXME 优化？
    in_data_ref.pop();
    return ret;
  }
  size_t getDatagramsNum() { return in_data_ref.size(); }

private:
  std::queue<Datagram> &in_data_ref;
};
class UdpServer : noncopyable {
public:
  UdpServer(EventLoop *loop, InetAddress listenAddr, string name);
  ~UdpServer();
  void start();
  void setMessageCallback(const UdpMessageCallback &cb) { message_cb_ = cb; }
  void setWriteAllCallback(const UdpWriteAllCallback &cb) { writeAll_cb_ = cb; }
  void send(const string& data,const InetAddress& target_addr);

private:
  static const int MAX_BUF_LEN = 548; /*为了在internet中较好传播*/
  void NetToBuffer();
  void BufferToNet();
  std::queue<Datagram> inBuffer_;
  std::queue<Datagram> outBuffer_;
  ; // 性能不大好
  UdpMessageCallback message_cb_;
  UdpWriteAllCallback writeAll_cb_;
  string name_;

  EventLoop *loop_;
  InetAddress addr_;
  int fd_;
  std::unique_ptr<Channel> channel_;
};
} // namespace feipu
#endif