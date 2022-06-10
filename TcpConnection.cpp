#include "TcpConnection.h"
#include <unistd.h>
namespace feipu {
TcpConnection::TcpConnection(Eventloop *loop, int connfd, InetAddress connAddr)
    : fd_(connfd), addr_(connAddr), loop_(loop),
      channel_(new Channel(fd_, loop_)) {
          channel_->setReadCall(std::bind(&TcpConnection::NetIntoBuffer,this));
          channel_->enableRead();
      }
    void TcpConnection::NetIntoBuffer(){
        //in_buffer <===== fd_
        inBuffer_.readFd(fd_);
        assert(message_cb_);
        message_cb_(shared_from_this(),&inBuffer_);// 回调messageCallback,这是从loop中调用的
    }
    void TcpConnection::BufferIntoNet(){
        //out_buffer =====> fd_
    }
    void TcpConnection::connectEstablished(){
        conn_cb_(shared_from_this());
    }
} // namespace feipu