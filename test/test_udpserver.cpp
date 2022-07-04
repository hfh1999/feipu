#include "UdpServer.h"
#include "EventLoop.h"
#include "Logging.h"
using namespace feipu;
void hear(DatagramStream& d)
{
    LOG_INFO << "datagram num = " << d.getDatagramsNum();
    auto num = d.getDatagramsNum();
    for(uint i = 0; i < num; i++)
    {
        auto data = d.reciveDatagram();
        auto n =  data.data_.size();
        printf("$%d  isvalid:%s ",i,(data.is_valid()?"yes":"no"));
        for(uint j = 0; j < n; j++)
        {
            printf("%c",data.data_[j]);
        }
        printf("\n");
    }
}
int main()
{
    Logger::setLogLevel(Logger::LoggerLevel::TRACE);
    EventLoop loop;
    UdpServer server(&loop,InetAddress(8888),"my_udp_server");
    server.setMessageCallback(hear);
    server.start();
    loop.loop();
}