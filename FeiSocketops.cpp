#include "FeiSocketops.h"
#include "Logging.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
namespace feipu {
namespace socket {
uint32_t hostToNetwork32(uint32_t hostlong) { return ::htonl(hostlong); }
uint16_t hostToNetwork16(uint16_t hostshort) { return ::htons(hostshort); }
uint32_t networkToHost32(uint32_t networklong) { return ::ntohl(networklong); }
uint16_t networkToHost16(uint16_t networkshort) {
  return ::ntohs(networkshort);
}

int createNonBlockingOrDie() {
  int sockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd < 0) {
    LOG_FATAL << "socket: create tcp socket error.";
  }

  // 设置阻塞
  int flags = ::fcntl(sockfd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  int ret = ::fcntl(sockfd, F_SETFL, flags);

  flags = ::fcntl(sockfd, F_GETFD, 0);
  flags |= FD_CLOEXEC;
  ret = ::fcntl(sockfd, F_SETFD, flags);
  if (ret < 0) {
    LOG_FATAL << "socket: set non-blcokong error.";
  }
  return sockfd;
}
int createUdpOrDie()
{
  int sockfd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sockfd < 0) {
    LOG_FATAL << "socket: create udp socket error.";
  }

  // 设置阻塞
  int flags = ::fcntl(sockfd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  int ret = ::fcntl(sockfd, F_SETFL, flags);
  if (ret < 0) {
    LOG_FATAL << "socket: set non-blcokong error.";
  }

  flags = ::fcntl(sockfd, F_GETFD, 0);
  flags |= FD_CLOEXEC;
  ret = ::fcntl(sockfd, F_SETFD, flags);
  if (ret < 0) {
    LOG_FATAL << "socket: set non-blcokong error.";
  }
  return sockfd;
}

int connect(int sockfd, const sockaddr_in &addr) {
  return ::connect(sockfd, reinterpret_cast<const struct sockaddr *>(&addr),
                   sizeof(addr));
}

void bindOrDie(int sockfd, const struct sockaddr_in &addr) {
  int ret = ::bind(sockfd, reinterpret_cast<const struct sockaddr *>(&addr),
                   sizeof(addr));
  if (ret < 0) {
    LOG_SYSFATAL << "sockets:bind error.";
  }
}

void listenOrDie(int sockfd) {
  int ret = ::listen(sockfd, SOMAXCONN);
  if (ret < 0) {
    LOG_FATAL << "sockets: listen error.";
  }
}

int accept(int sockfd, struct sockaddr_in *addr) {
  socklen_t addrlen = sizeof(*addr);
  return ::accept(sockfd, reinterpret_cast<struct sockaddr *>(addr), &addrlen);
}
void closeOrDie(int sockfd) {
  int ret = ::close(sockfd);
  if (ret < 0) {
    LOG_FATAL << "sockets: close error.";
  }
}
void shutdownWrite(int sockfd) {
  if (::shutdown(sockfd, SHUT_WR) < 0) {
    LOG_SYSERROR << "sockets: shutdownWrite error.";
  }
}
void fromHostPort(const char *ip, uint16_t port, struct sockaddr_in *addr) {
  addr->sin_family = AF_INET;
  addr->sin_port = port;
  if (inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
    LOG_FATAL << "sockets: inet_pton error.";
  }
}
} // namespace socket

} // namespace feipu