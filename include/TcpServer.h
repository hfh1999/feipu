#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "tools.h"
namespace feipu {
class Eventloop;
// 单线程的tcpServer
class TcpServer:noncopyable
{
    public:
    TcpServer(Eventloop* loop);

};
}
#endif