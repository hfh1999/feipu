#ifndef UDPSOCKET_H
#define UDPSOCKET_H
#include "tools.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include "FeiTypes.h"
#include "Channel.h"
namespace feipu {
    class UdpService:noncopyable
    {
        public:
        UdpService(EventLoop* loop,InetAddress peeraddr)
        :loop_(loop),peerAddr_(peeraddr)
        {
        }

        private:
        EventLoop* loop_;
        InetAddress peerAddr_;
        std::unique_ptr<Channel> channel_;
    };
}
#endif