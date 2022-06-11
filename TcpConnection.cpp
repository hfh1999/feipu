#include "TcpConnection.h"
#include <unistd.h>
#include "FeiSocketops.h"
namespace feipu {
TcpConnection::TcpConnection(Eventloop *loop, int connfd, InetAddress connAddr)
    : fd_(connfd), addr_(connAddr), loop_(loop),
      channel_(new Channel(fd_, loop_)) {
  channel_->setReadCall(std::bind(&TcpConnection::NetIntoBuffer, this));
}
TcpConnection::~TcpConnection(){
    socket::closeOrDie(fd_);
}
void TcpConnection::NetIntoBuffer() {
  // in_buffer <===== fd_
  size_t n = inBuffer_.readFd(fd_);
  if (n == 0) // 断开事件
  {
    assert(close_cb_);
    close_cb_(shared_from_this());
  } else { // message事件
    assert(message_cb_);
    message_cb_(shared_from_this(),
                &inBuffer_); // 回调messageCallback,这是从loop中调用的
  }
}
void TcpConnection::BufferIntoNet() {
  // out_buffer =====> fd_
}
void TcpConnection::connectEstablished() {
  conn_cb_(shared_from_this());// FIXME 顺序思考下
  channel_->enableRead();
}
} // namespace feipu