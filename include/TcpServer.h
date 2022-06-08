#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "tools.h"
namespace feipu {
class Eventloop;
class TcpServer:noncopyable
{
    public:
    TcpServer(Eventloop* loop);

};
}
#endif