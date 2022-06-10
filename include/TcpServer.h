#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "tools.h"
#include "InetAddress.h"
#include "Channel.h"
#include "FeiSocketops.h"
#include <arpa/inet.h>
#include "Logging.h"
namespace feipu {
class Eventloop;

/*用于接受连接*/
class Acceptor{
public:
typedef std::function<void(int,InetAddress)> NewConnectionCallback;
Acceptor(Eventloop* loop,InetAddress listenAddr)
:sockfd_(socket::createNonBlockingOrDie()),
channel_(new Channel(sockfd_,loop))
{
    socket::bindOrDie(sockfd_, listenAddr.getRawSockAddr());
    LOG_INFO << "has bind";
}
void start(){
    socket::listenOrDie(sockfd_);
    channel_->setReadCall(std::bind(&Acceptor::handleNewConnect,this));// 有新连接进入时
    channel_->enableRead();
}
void setNewConnectionCallback(NewConnectionCallback in_cb){
    cb_ = in_cb;
}
private:
void handleNewConnect(){
  struct sockaddr_in remote_address_raw;
  int remotefd = socket::accept(sockfd_, &remote_address_raw);
  if (remotefd > 0) {
    if (cb_) {
      cb_(remotefd, InetAddress(remote_address_raw));
    } else {
      socket::closeOrDie(remotefd);
    }
  }
}
int sockfd_;
Channel * channel_;
NewConnectionCallback cb_;
};
// 单线程的tcpServer
class TcpServer:noncopyable
{
    public:
    TcpServer(Eventloop* loop,InetAddress listenAddr,string name);
    void start();
    void setConnectionCallback();
    void setMessageCallback();
    void setWriteAllCallback();
    private:
    void whenNewConnection(int remoteFd,InetAddress remoteAddr);
    InetAddress addr_;
    string name_;
    Acceptor acceptor_;
};
}
#endif