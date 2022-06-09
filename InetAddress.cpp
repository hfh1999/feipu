#include "InetAddress.h"
#include <cstring>
#include "FeiSocketops.h"
#include <netinet/in.h>
namespace feipu {
InetAddress::InetAddress(int32_t port)
:addr_(new sockaddr_in)
{
    ::bzero(addr_.get(),sizeof(*addr_));
    addr_->sin_family = AF_INET;
    addr_->sin_port = port;
    addr_->sin_addr.s_addr = socket::hostToNetwork32(INADDR_ANY);
}
InetAddress::InetAddress(const string& ip, uint16_t port)
:addr_(new sockaddr_in)
{
    ::bzero(addr_.get(),sizeof(*addr_));
    socket::fromHostPort(ip.c_str(), port, addr_.get());
}
InetAddress::InetAddress(const struct sockaddr_in& addr)
:addr_(new sockaddr_in(addr))
{
}
}