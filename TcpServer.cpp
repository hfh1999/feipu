#include "TcpServer.h"
#include "CallBack.h"
#include "FeiTypes.h"
#include "Logging.h"
#include "TcpConnection.h"
namespace feipu {
TcpServer::TcpServer(EventLoop *loop, InetAddress listenAddr, string name)
    : loop_(loop), addr_(listenAddr), name_(name), acceptor_(loop, listenAddr),
      threads_pool_(loop) {}
void TcpServer::start() {
  LOG_INFO << "[TcpServer:" << name_ << "]"
           << "start.";
  acceptor_.setNewConnectionCallback(
      std::bind(&TcpServer::whenNewConnection, this, _1, _2));
  acceptor_.start();
  threads_pool_.start();
}
void TcpServer::whenNewConnection(int remoteFd, InetAddress remoteAddr) {
  LOG_TRACE << "TcpConnection sum = " << connections_.size();
  LOG_INFO << "[TcpServer: " << name_ << "] New Connection.";

  // 1. 创建connection对象去管理connection(同时绑定read和write回调)
  // 注意这里通过enable_from_this让所有的share_ptr共用一个引用计数
  InetAddress localAddr = InetAddress::GetLocalAddr(remoteFd);
  EventLoop *nextloop = threads_pool_.getNextLoop();
  TcpConnectionPtr newConn(
      new TcpConnection(nextloop, remoteFd, localAddr, remoteAddr));
  newConn->setConnectionCallback(conn_cb_);
  newConn->setCloseCallback(
      std::bind(&TcpServer::whenOldConnDisconnect, this, _1));
  newConn->setMessageCallback(message_cb_);
  newConn->setWriteCallback(write_cb_);

  // 2. 所有权记录
  connections_.insert(newConn);//FIXME 应该用fd作键

  // 3. 回调connectionCallback.
  newConn->getloop()->runInLoop(
      std::bind(&TcpConnection::connectEstablished, newConn)); // 转移到相应线程运行
}

void TcpServer::whenOldConnDisconnect(TcpConnectionPtr conn) {
  // 删除所有权记录即可
  LOG_INFO << "[TcpServer: " << name_ << "] Connection Disconnected.";
  size_t n = connections_.count(conn);
  assert(n != 0); // 必然有
  connections_.erase(conn);
  /*user call*/
  conn_cb_(conn); // 断开连接时也调用connectCallBack.
}
void TcpServer::setThreadNum(unsigned short num) { threads_pool_.setNum(num); }
} // namespace feipu