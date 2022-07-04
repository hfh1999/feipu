#include "InetAddress.h"
#include "FeiSocketops.h"
#include "Logging.h"
#include <cstring>
#include <netinet/in.h>
namespace feipu {
InetAddress::InetAddress(int16_t port) : addr_(new sockaddr_in) {
  ::bzero(addr_.get(), sizeof(*addr_));
  addr_->sin_family = AF_INET;
  addr_->sin_port = socket::hostToNetwork16(port);
  addr_->sin_addr.s_addr = socket::hostToNetwork32(INADDR_ANY);
}
InetAddress::InetAddress() : addr_() {}
InetAddress::InetAddress(const string &ip, uint16_t port)
    : addr_(new sockaddr_in) {
  ::bzero(addr_.get(), sizeof(*addr_));
  socket::fromHostPort(ip.c_str(), port, addr_.get());
}
InetAddress::InetAddress(const struct sockaddr_in &addr)
    : addr_(new sockaddr_in(addr)) {}
InetAddress::InetAddress(const InetAddress &to_copy)
    : addr_(to_copy.addr_?new sockaddr_in(*(to_copy.addr_)):nullptr) {}
InetAddress &InetAddress::operator=(const InetAddress &rhs) {
  // FIXME: 写的太烂

  /*处理自我赋值*/
  if (&addr_ == &(rhs.addr_)) {
    return *this;
  }
  /*addr_可能为空*/
  if (addr_.get() == nullptr) {
    auto tmp_ptr = new sockaddr_in();
    addr_.reset(tmp_ptr);
  }
  /*rhs可能为空*/
  if (rhs.addr_.get() == nullptr) {
    auto tmp_ptr = addr_.release();
    delete tmp_ptr;
    return *this;
  }
  *addr_ = *(rhs.addr_);
  return *this;
}
InetAddress::~InetAddress() {}
InetAddress InetAddress::GetPeerAddr(int fd) {
  struct sockaddr_in localaddr;
  ::bzero(&localaddr, sizeof(localaddr));
  socklen_t addrlen = sizeof(localaddr);
  if (::getpeername(fd, reinterpret_cast<struct sockaddr *>(&localaddr),
                    &addrlen) < 0) {
    LOG_FATAL << "InetAddress:GetPeerAddr() error.";
  }
  return InetAddress(localaddr);
}
InetAddress InetAddress::GetLocalAddr(int fd) {
  struct sockaddr_in localaddr;
  ::bzero(&localaddr, sizeof(localaddr));
  socklen_t addrlen = sizeof(localaddr);
  if (::getsockname(fd, reinterpret_cast<struct sockaddr *>(&localaddr),
                    &addrlen) < 0) {
    LOG_FATAL << "InetAddress:GetPeerAddr() error.";
  }
  return InetAddress(localaddr);
}
bool InetAddressComp::operator()(const InetAddress &lhs,
                                 const InetAddress &rhs) {
  /*待优化*/
  unsigned int addr_num_l = lhs.addr_->sin_addr.s_addr;
  unsigned short port_num_l = lhs.addr_->sin_port;
  unsigned int addr_num_r = rhs.addr_->sin_addr.s_addr;
  unsigned short port_num_r = rhs.addr_->sin_port;
  int64_t sum_l = addr_num_l * 10 + port_num_l;
  int64_t sum_r = addr_num_r * 10 + port_num_r;
  return sum_l < sum_r;
}
} // namespace feipu