#include "TcpServer.h"
#include "CallBack.h"
#include "Logging.h"
#include "TcpConnection.h"
namespace feipu {
TcpServer::TcpServer(Eventloop *loop, InetAddress listenAddr, string name)
    : loop_(loop),addr_(listenAddr), name_(name),acceptor_(loop,listenAddr) {}
void TcpServer::start(){
    LOG_INFO << "[TcpServer:" << name_ << "]" << "start.";
    acceptor_.setNewConnectionCallback(std::bind(&TcpServer::whenNewConnection,this,_1,_2));
    acceptor_.start();
}
void TcpServer::whenNewConnection(int remoteFd,InetAddress remoteAddr){
    LOG_INFO << "Recieve Connect.";
    socket::closeOrDie(remoteFd);

    //1. 创建connection对象去管理connection(同时绑定read和write回调)
    TcpConnection* newConn = new TcpConnection(loop_,remoteFd,remoteAddr);
    newConn->setMessageCallback(message_cb_);
    //2. 回调connectionCallback.
    newConn->connectEstablished();
}
} // namespace feipu