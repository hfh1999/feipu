#include "EventLoop.h"
#include "HttpServer.h"
using namespace feipu;
int main()
{
    EventLoop loop;
    HttpServer server(&loop,InetAddress(8989),"first web server.");
    server.start();
    loop.loop();
    return 0;
}