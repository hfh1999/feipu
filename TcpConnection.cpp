#include "TcpConnection.h"
#include "FeiSocketops.h"
#include "Logging.h"
#include <unistd.h>
namespace feipu {
TcpConnection::TcpConnection(Eventloop *loop, int connfd, InetAddress localaddr,
                             InetAddress peeraddr)
    : fd_(connfd), localaddr_(localaddr), peeraddr_(peeraddr), loop_(loop),
      channel_(new Channel(fd_, loop_)), status_(ConnStatus::Disconnected) {
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
  loop_->assertInLoopThread();
  ssize_t n = inBuffer_.readFd(fd_);
  if (n == 0) // 断开事件
  {
    handleClose();
  } else if (n > 0) { // message事件
    if (message_cb_) {
      message_cb_(shared_from_this(),
                  &inBuffer_); // 回调messageCallback,这是从loop中调用的
    }
  } else {
    // 发生错误了
  }
}
void TcpConnection::send(const char *data, size_t len) {
  if (status_ != ConnStatus::Connected) {
    LOG_WARN << "TcpConnection::send() try to send data when connection has "
                "been disconnected.";
    return;
  }
  if (loop_->isInLoopThread()) {
    sendInLoop(data, len);
  } else {
    loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, data, len));
  }
}
void TcpConnection::send(const string &data) {
  if (status_ != ConnStatus::Connected) {
    LOG_WARN << "TcpConnection::send() try to send data when connection has "
                "been disconnected.";
    return;
  }
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
  loop_->assertInLoopThread();
  int n_write;
  if (outBuffer_.getReadableBytes() == 0 && !channel_->isWriting()) {
    LOG_TRACE << "Directly send data.";
    n_write = ::write(channel_->fd(), data, len);
    if (n_write < 0) {
      LOG_SYSERROR << "TcpConnection::sendInLoop";
    } else if (static_cast<size_t>(n_write) < len) {
      LOG_TRACE << "will send more data."
                << " n = " << n_write << "has send.";
      outBuffer_.append(data + n_write, len - n_write);
      channel_->enableWrite();
    } else // 全发送完则发出writeCallback
    {
      // FIXME 到底是立即执行还是写到functions_等下一轮执行？
      LOG_TRACE << "direct send all data.";
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
  loop_->assertInLoopThread();
  if (channel_->isWriting() == false) {
    //当连接被对方关闭时，fd会变为无效，此时不必再写了，否则会造成不正常的错误和退出
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
      if (status_ == ConnStatus::Disconnecting) {
        shutInLoop();
      }
    }
  } else {
    LOG_FATAL << "TcpConnection: write error."; // 需要考虑下
  }
}
void TcpConnection::connectEstablished() {
  channel_->enableRead();
  channel_->tie(shared_from_this());
  status_ = ConnStatus::Connected;
  conn_cb_(shared_from_this()); // FIXME 顺序思考下
}
void TcpConnection::handleClose() {
  assert(close_cb_);
  LOG_TRACE << "disconnected.";
  status_ = ConnStatus::Disconnected;
  channel_->disableRead();
  channel_->disableWrite();
  close_cb_(shared_from_this()); // 回调tcpServer中的销毁过程
}
void TcpConnection::shutdown() {
  if (status_ == ConnStatus::Connected) {
    status_ = ConnStatus::
        Disconnecting; // 保证尽早变状态(否则进入队列要排队等待才能变状态)
    loop_->runInLoop(std::bind(&TcpConnection::shutInLoop, this));
  }
}
void TcpConnection::shutInLoop() {
  loop_->assertInLoopThread();
  if (channel_->isWriting()) {
    LOG_FATAL << "TcpConnection::shutInloop() should be call when the data is "
                 "not writing.";
  }
  socket::shutdownWrite(channel_->fd()); // 等数据发完再shutdown
  LOG_TRACE << "TcpConnection:  has shut down.";
}
} // namespace feipu