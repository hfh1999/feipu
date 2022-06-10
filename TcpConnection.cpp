#include "TcpConnection.h"
namespace feipu {
TcpConnection::TcpConnection(Eventloop *loop, int connfd, InetAddress connAddr)
    : fd_(connfd), addr_(connAddr), loop_(loop),
      channel_(new Channel(fd_, loop_)) {
          channel_->setReadCall(std::bind(&TcpConnection::NetIntoBuffer,this));
          channel_->enableRead();
      }
    void TcpConnection::NetIntoBuffer(){
        //in_buffer <===== fd_
    }
    void TcpConnection::BufferIntoNet(){
        //out_buffer =====> fd_
    }
} // namespace feipu