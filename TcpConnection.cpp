#include "TcpConnection.h"
#include "FeiSocketops.h"
#include "Logging.h"
#include <unistd.h>
namespace feipu {
TcpConnection::TcpConnection(Eventloop *loop, int connfd, InetAddress localaddr,
                             InetAddress peeraddr)
    : fd_(connfd),localaddr_(localaddr),peeraddr_(peeraddr), loop_(loop),
      channel_(new Channel(fd_, loop_)) {
  channel_->setReadCall(std::bind(&TcpConnection::NetIntoBuffer, this));
  channel_->setWriteCall(std::bind(&TcpConnection::BufferIntoNet, this));
}
TcpConnection::~TcpConnection() {
  socket::closeOrDie(fd_);
  LOG_TRACE << "TcpConnection Deleted.";
  channel_->disableRead();  // delete?
  channel_->disableWrite(); // delete?
  channel_->un_register();  // TcpConnection析构时必须解注册
                            // FIXME 考虑在close事件刚发生时就进行解注册
}
void TcpConnection::NetIntoBuffer() {
  // in_buffer <===== fd_
  size_t n = inBuffer_.readFd(fd_);
  if (n == 0) // 断开事件
  {
    handleClose();
  } else { // message事件
    if (message_cb_) {
      message_cb_(shared_from_this(),
                  &inBuffer_); // 回调messageCallback,这是从loop中调用的
    }
  }
}
void TcpConnection::send(const char *data, size_t len) {
  if (loop_->isInLoopThread()) {
    sendInLoop(data, len);
  } else {
    loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, data, len));
  }
}
void TcpConnection::send(const string &data) {
  if (loop_->isInLoopThread()) {
    sendInLoop(data.data(), data.size());
  } else {
    loop_->runInLoop(
        std::bind(&TcpConnection::sendInLoop, this, data.data(), data.size()));
  }
}
void TcpConnection::sendInLoop(const char *data, size_t len) {
  // 当outbuffer没有内容时尝试这里直接发送操作
  /*
     buffer   sys-buffer
  1. [     ]=>[xxxxx]
  2. [  xxx]=>[xxxxx]
  3. [     ]=>[     ]
  只有第三种情况才能直接输出
  */
  // TODO 这里的isWriting好好想想
  assert(loop_->isInLoopThread());
  int n_write;
  if (outBuffer_.getReadableBytes() == 0 && !channel_->isWriting()) {
    LOG_TRACE << "Directly send data.";
    n_write = ::write(channel_->fd(), data, len);
    if (n_write < 0) {
      LOG_FATAL << "TcpConnection: Write error.";
    } else if (static_cast<size_t>(n_write) < len) {
      LOG_TRACE << "will send more data.";
      outBuffer_.append(data + n_write, len - n_write);
      channel_->enableWrite();
    } else // 全发送完则发出writeCallback
    {
      // FIXME 到底是立即执行还是写到functions_等下一轮执行？
      if (write_cb_) {
        write_cb_(shared_from_this());
      }
    }
  } else {
    outBuffer_.append(data, len);
    channel_->enableWrite();
  }
  if (channel_->isWriting()) {
    LOG_TRACE << "is Writing.";
  }
}
// void TcpConnection::sendInLoop(const string &data) {
//  int n_write;
//  if (outBuffer_.getReadableBytes() == 0 && !channel_->isWriting()) {
//    LOG_TRACE << "Directly send data.";
//    n_write = ::write(channel_->fd(), data.data(), data.size());
//    if (n_write < 0) {
//      LOG_FATAL << "TcpConnection: Write error.";
//    } else if (static_cast<size_t>(n_write) <= data.size()) {
//      LOG_TRACE << "will send more data.";
//      outBuffer_.append(data.data() + n_write, data.size() - n_write);
//      channel_->enableWrite();
//    } else // 全发送完则什么也不做
//    {
//      //FIXME 到底是立即执行还是写到functions_等下一轮执行？
//      if(write_cb_)
//      {
//        write_cb_(shared_from_this());
//      }
//    }
//  } else {
//    outBuffer_.append(data);
//    channel_->enableWrite();
//  }
//  outBuffer_.append(data);
//  channel_->enableWrite();
//}
void TcpConnection::BufferIntoNet() {
  // out_buffer =====> fd_
  if (channel_->isWriting() == false) {
    LOG_TRACE << "Connection has close,return.";
    return;
  }
  size_t write_n = outBuffer_.getReadableBytes();
  int n = ::write(fd_, outBuffer_.peek(), write_n);
  LOG_TRACE << "towrite n = " << write_n << " ,write n = " << n;
  if (n > 0) {
    outBuffer_.retrieve(n);
    if (outBuffer_.getReadableBytes() == 0) // buffer已经全部输出完成
    {
      channel_->disableWrite();
      if (write_cb_) {
        // FIXME 到底是立即执行还是写到functions_等下一轮执行？
        write_cb_(shared_from_this());
      }
    }
  } else {
    LOG_FATAL << "TcpConnection: write error."; // 需要考虑下
  }
}
void TcpConnection::connectEstablished() {
  channel_->enableRead();
  channel_->tie(shared_from_this());
  conn_cb_(shared_from_this()); // FIXME 顺序思考下
}
void TcpConnection::handleClose() {
  assert(close_cb_);
  LOG_TRACE << "disconnected.";
  channel_->disableRead();
  channel_->disableWrite();
  close_cb_(shared_from_this());
}
} // namespace feipu