#ifndef inetaddress_h
#define inetaddress_h
#include "FeiTypes.h"
#include "tools.h"
class sockaddr_in;
namespace feipu {
class InetAddress:copyable{
    public:
    explicit InetAddress(int16_t port);

    InetAddress();
    InetAddress(const string& ip , uint16_t port);
    InetAddress(const struct sockaddr_in& addr);
    InetAddress(const InetAddress&); // copy
    InetAddress& operator=(const InetAddress&); // assign;
    static InetAddress GetPeerAddr(int fd);
    static InetAddress GetLocalAddr(int fd);
    const sockaddr_in& getRawSockAddr() const {return *addr_;}
    string toIpPort() const;
    string toIp() const;
    bool is_valid(){
        if(addr_.get() == nullptr)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    ~InetAddress();
    private:
    friend class InetAddressComp;
    std::unique_ptr<sockaddr_in> addr_; // 这是为了不暴露底层的数据
};
class InetAddressComp{
    bool operator()(const InetAddress& lhs,const InetAddress& rhs);
};
}
#endif