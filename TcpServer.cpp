#include "TcpServer.h"
#include "CallBack.h"
#include "Logging.h"
#include "TcpConnection.h"
#include "FeiTypes.h"
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

    //1. 创建connection对象去管理connection(同时绑定read和write回调)
    // 注意这里通过enable_from_this让所有的share_ptr共用一个引用计数
    TcpConnectionPtr newConn(new TcpConnection(loop_,remoteFd,remoteAddr));
    newConn->setConnectionCallback(conn_cb_);
    newConn->setCloseCallback(std::bind(&TcpServer::whenOldConnDisconnect,this,_1));
    newConn->setMessageCallback(message_cb_);
    newConn->setWriteCallback(write_cb_);

    //2. 所有权记录
    connections_.insert(newConn);

    //3. 回调connectionCallback.
    newConn->connectEstablished();// 在TcpServer的线程中
    // conn_cb_(newConn); 也可以
}

void TcpServer::whenOldConnDisconnect(TcpConnectionPtr conn){
    // 删除所有权记录即可
    size_t n = connections_.count(conn);
    assert(n != 0); // 必然有
    connections_.erase(conn);
    conn_cb_(conn); // 断开连接时也调用connectCallBack.
}
} // namespace feipu