#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H
#include "InetAddress.h"
#include "FeiTypes.h"
#include "EventLoop.h"
#include "Channel.h"
#include "CallBack.h"
namespace feipu{
    /*本类应该具有自动化的生命周期管理*/
    /*负责管理缓冲区*/
    class TcpConnection{
        public:
        TcpConnection(Eventloop* loop,int connfd,InetAddress connAddr);
        void setConnectionCallback(const ConnectionCallback& cb) {conn_cb_ = cb;}
        void setMessageCallback(const MessageCallback& cb){message_cb_ = cb;}
        private:
        void NetIntoBuffer();// 将网络中的数据读入buffer中
        void BufferIntoNet();// 将buffer的数据写入到网络之中
        int fd_;
        InetAddress addr_;
        Eventloop* loop_;
        MessageCallback message_cb_;
        ConnectionCallback conn_cb_;
        std::unique_ptr<Channel> channel_;
   };
};
#endif