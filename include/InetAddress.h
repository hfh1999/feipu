#ifndef inetaddress_h
#define inetaddress_h
#include "FeiTypes.h"
#include "tools.h"
class sockaddr_in;
namespace feipu {
class InetAddress:copyable{
    public:
    explicit InetAddress(int32_t port);

    InetAddress(const string& ip , uint16_t port);
    InetAddress(const struct sockaddr_in& addr);
    private:
    std::unique_ptr<sockaddr_in> addr_; // 这是为了不暴露底层的数据
};
}
#endif