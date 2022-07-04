#include "UdpServer.h"
#include "Channel.h"
#include "FeiSocketops.h"
#include "Logging.h"
#include <arpa/inet.h>
namespace feipu {
UdpServer::UdpServer(EventLoop *loop, InetAddress listenAddr, string name)
    : loop_(loop), addr_(listenAddr), name_(name),
      fd_(socket::createUdpOrDie()), channel_(new Channel(fd_, loop)) {
  socket::bindOrDie(fd_, addr_.getRawSockAddr());
}
UdpServer::~UdpServer() {
  LOG_INFO << "[UdpServer:" << name_ << "]"
           << "End Server";
  socket::closeOrDie(fd_);
}
void UdpServer::start() {
  channel_->setReadCall(std::bind(&UdpServer::NetToBuffer, this));
  channel_->setWriteCall(std::bind(&UdpServer::BufferToNet, this));
  channel_->enableRead();
  LOG_INFO << "[UdpServer:" << name_ << "]"
           << "Start Server";
}
void UdpServer::NetToBuffer() {
  char buffer[MAX_BUF_LEN];
  sockaddr_in addr;
  socklen_t len = sizeof(addr);
  int count = 0;
  while (true) {
    ssize_t n = ::recvfrom(fd_, buffer, sizeof(buffer), 0,
                           reinterpret_cast<sockaddr *>(&addr), &len);
    if (n < 0) {
      if (errno == 11) // 对非阻塞而言这不是错误
      {
        break;
      } else {
        /*read error.*/
        // debug
        LOG_SYSFATAL << "UdpServer: read udp error.";
      }
    } else {
      /*n == 0表示收入长度为0的数据报*/
      count += 1;
      string tmp_buffer(buffer, buffer + n);
      Datagram data;
      data.addr_ = InetAddress(addr);
      data.data_.swap(tmp_buffer);
      inBuffer_.push(std::move(data));
    }
  }
  LOG_TRACE << "Recieve " << count << " Udp Datagrams.";
  if (count > 0) {
    if (message_cb_) {
      DatagramStream ss(inBuffer_);
      /*注意message_cb_中线程不安全*/
      message_cb_(ss);
    }
  }
}
void UdpServer::send(const string &data, const InetAddress &target_addr) {
  // TODO 可以立刻写，假如可写的话
  Datagram tmp_data;
  tmp_data.data_ = data;
  tmp_data.addr_ = target_addr;
  outBuffer_.push(std::move(tmp_data));
  channel_->enableWrite();
}
void UdpServer::BufferToNet() {
  /*要么完整发送要么不发送*/
  assert(outBuffer_.size() > 0);
  assert(channel_->isWriting());
  while (true) {

    auto tmp_data = outBuffer_.front().data_;
    auto target_addr = outBuffer_.front().addr_.getRawSockAddr();
    socklen_t socklen = sizeof(target_addr);
    ssize_t n = ::sendto(fd_, &tmp_data[0], tmp_data.size(), 0,
                         reinterpret_cast<sockaddr *>(&target_addr), socklen);
    if (n < 0) {
      if (errno == 11) // FIXME: ???
        return;
      LOG_SYSFATAL << "UdpServer: error when write.";
    } else {
      if (static_cast<size_t>(n) < tmp_data.size()) {
        //没发成功
        return;
      } else {
        //发成功了
        outBuffer_.pop();
      }
    }
    size_t n_packets = outBuffer_.size();
    if (n_packets > 0) {
      assert(channel_->isWriting());
    } else // == 0
    {
      channel_->disableWrite();
      if (writeAll_cb_) {
        writeAll_cb_();
      }
    }
  }
}
} // namespace feipu