/*测试TcpServer*/
#include "Buffer.h"
#include "EventLoop.h"
#include "Logging.h"
#include "TcpConnection.h"
#include "TcpServer.h"
using feipu::Buffer;
using feipu::Logger;
using feipu::TcpConnectionPtr;
using feipu::TcpServer;
void whenConnected(TcpConnectionPtr conn) {
  LOG_INFO << "Connected,or Disconncted.";
  //feipu::string message("hello from server.");
  //conn->send(message); // FIXME bug in send.
  feipu::string message("hello from server.");
  conn->send(message);// FIXME bug in send.
}
void whenRead(TcpConnectionPtr conn, Buffer *buffer) {
  auto n = buffer->getReadableBytes();
  LOG_INFO << "has " << n << " bytes to read.";
  LOG_INFO << "read index = " << buffer->ret_readIndex() << ","
           << "write index = " << buffer->ret_writeIndex() << ","
           << "data size = " << buffer->ret_buffersize();
  // LOG_INFO << "That is " << feipu::string(buffer->peek(),n);
  buffer->retrieve(n);
}
void whenWriteAll(TcpConnectionPtr conn) { LOG_INFO << "Send ALL"; }
int main() {
  Logger::setLogLevel(Logger::TRACE);
  feipu::Eventloop myloop;
  feipu::TcpServer server(&myloop, feipu::InetAddress(7788), "test_server");
  server.setConnectionCallback(whenConnected);
  server.setMessageCallback(whenRead);
  server.setWriteAllCallback(whenWriteAll);
  server.start();
  myloop.loop();
}