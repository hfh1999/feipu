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
    LOG_TRACE << "TcpConnction: TcpConnection Deleted.";
    channel_->disableRead();// delete?
    channel_->disableWrite();// delete?
    channel_->un_register();// TcpConnection析构时必须解注册
}
void TcpConnection::NetIntoBuffer() {
  // in_buffer <===== fd_
  size_t n = inBuffer_.readFd(fd_);
  if (n == 0) // 断开事件
  {
    handleClose();
  } else { // message事件
    if(message_cb_)
    {
      message_cb_(shared_from_this(),
                  &inBuffer_); // 回调messageCallback,这是从loop中调用的
    }
  }
}
void TcpConnection::send(const char *data, size_t len){
  // 当outbuffer没有内容时尝试这里直接发送操作
  int n_write;
  /*
     buffer   sys-buffer
  1. [     ]=>[xxxxx]
  2. [  xxx]=>[xxxxx]
  3. [     ]=>[     ]
  只有第三种情况才能直接输出
  */
  //TODO 这里的isWriting好好想想
  if(outBuffer_.getReadableBytes() == 0 && !channel_->isWriting())
  {
    LOG_TRACE << "TcpConnection::send: Directly send data.";
    n_write = ::write(channel_->fd(),data,len);
    if(n_write < 0)
    {
      LOG_FATAL << "TcpConnection: Write error.";
    }
    else if(static_cast<size_t>(n_write) <= len)
    {
      LOG_TRACE << "TcpConnection::send: will send more data.";
      outBuffer_.append(data + n_write,len - n_write);
      channel_->enableWrite();
    }
    else// 全发送完则什么也不做
    {
    }
  }
  else
  {
    outBuffer_.append(data,len);
    channel_->enableWrite();
  }
}
void TcpConnection::send(const string& data)
{
  LOG_TRACE << "appending data";
  outBuffer_.append(data);
  channel_->enableWrite();
  if(channel_->isWriting())
  {
    LOG_TRACE << "is Writing.";
  }
}
void TcpConnection::BufferIntoNet() {
  // out_buffer =====> fd_
  if(channel_->isWriting() == false)
  {
    LOG_FATAL << "TcpConnection::BufferIntoNet: must be writing.";
  }
  size_t write_n = outBuffer_.getReadableBytes();
  int n = ::write(fd_, outBuffer_.peek(),write_n);
  LOG_TRACE << "TcpConnection: towrite n = "<< write_n << "write n = " << n;
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
    LOG_FATAL << "TcpConnection: write error.";// 需要考虑下
  }

}
void TcpConnection::connectEstablished() {
  channel_->enableRead();
  conn_cb_(shared_from_this());// FIXME 顺序思考下
}
void TcpConnection::handleClose()
{
    assert(close_cb_);
    LOG_TRACE << "TcpConnection: disconnected.";
    close_cb_(shared_from_this());
}
} // namespace feipu