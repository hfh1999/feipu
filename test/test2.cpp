/*测试TcpServer*/
#include "EventLoop.h"
#include "TcpServer.h"
#include "Logging.h"
#include "Buffer.h"
using feipu::TcpServer;
using feipu::TcpConnectionPtr;
using feipu::Logger;
using feipu::Buffer;
void whenConnected(TcpConnectionPtr conn)
{
    LOG_INFO << "Connected,or Disconncted.";
}
void whenRead(TcpConnectionPtr conn,Buffer* buffer)
{
    auto n = buffer->getReadableBytes();
    LOG_INFO << "has " << n << " bytes to read.";
    LOG_INFO << "read index = " << buffer->ret_readIndex()<< "," << "write index = " << buffer->ret_writeIndex()<<","<<"data size = " << buffer->ret_buffersize();
    //LOG_INFO << "That is " << feipu::string(buffer->peek(),n);
    buffer->retrieve(n);
}
int main()
{
    //Logger::setLogLevel(Logger::TRACE);
    feipu::Eventloop myloop;
    feipu::TcpServer server(&myloop,feipu::InetAddress(7788),"test_server");
    server.setConnectionCallback(whenConnected);
    server.setMessageCallback(whenRead);
    server.start();
    myloop.loop();
}