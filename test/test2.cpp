/*测试TcpServer*/
#include "EventLoop.h"
#include "TcpServer.h"

int main()
{
    feipu::Eventloop myloop;
    feipu::TcpServer server(&myloop,feipu::InetAddress(7788),"test_server");
    server.start();
    myloop.loop();
}