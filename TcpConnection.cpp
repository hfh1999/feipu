#include "TcpConnection.h"
#include <unistd.h>
#include "FeiSocketops.h"
#include "Logging.h"
namespace feipu {
TcpConnection::TcpConnection(Eventloop *loop, int connfd, InetAddress connAddr)
    : fd_(connfd), addr_(connAddr), loop_(loop),
      channel_(new Channel(fd_, loop_)) {
  channel_->setReadCall(std::bind(&TcpConnection::NetIntoBuffer, this));
  channel_->setWriteCall(std::bind(&TcpConnection::BufferIntoNet,this));
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
void TcpConnection::send(const char *data, size_t len){
  outBuffer_.append(data,len);
  channel_->enableWrite();
}
void TcpConnection::BufferIntoNet() {
  // out_buffer =====> fd_
  assert(true); // 必须是enableRead状态
  size_t write_n = outBuffer_.getReadableBytes();
  int n = ::write(fd_, outBuffer_.peek(),write_n);
  if(n > 0)
  {
    outBuffer_.retrieve(n);
    if(outBuffer_.getReadableBytes() == 0) // buffer已经全部输出完成
    {
      channel_->disableWrite();
      if(write_cb_)
      {
        write_cb_(shared_from_this());
      }
    }
  }
  else
  {
    LOG_FATAL << "TcpConnection: write error.";
  }

}
void TcpConnection::connectEstablished() {
  conn_cb_(shared_from_this());// FIXME 顺序思考下
  channel_->enableRead();
}
} // namespace feipu