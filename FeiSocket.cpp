#include "FeiSocket.h"
#include <sys/socket.h>
#include "Logging.h"
#include <fcntl.h>
#include <unistd.h>
namespace feipu {
namespace socket {
uint32_t hostToNetwork32(uint32_t hostlong) { return ::htonl(hostlong); }
uint16_t hostToNetwork16(uint16_t hostshort) { return ::htons(hostshort); }
uint32_t networkToHost32(uint32_t networklong) { return ::ntohl(networklong); }
uint16_t networkToHost16(uint16_t networkshort) {
  return ::ntohs(networkshort);
}

int createNonBlockingOrDie(){
    int sockfd = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(sockfd < 0)
    {
        LOG_FATAL << "socket: create socket error.";
    }

    // 设置阻塞
    int flags = ::fcntl(sockfd,F_GETFL,0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd,F_SETFL,flags);

    flags = ::fcntl(sockfd,F_GETFD,0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(sockfd,F_SETFD,flags);
    if(ret < 0)
    {
        LOG_FATAL << "socket: set non-blcokong error.";
    }
    return sockfd;
}


int connect(int sockfd,const struct sockaddr_in& addr){
    return ::connect(sockfd, reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr));
}

void bindOrDie(int sockfd, const struct sockaddr_in &addr)
{
    int ret = ::bind(sockfd,reinterpret_cast<const struct sockaddr*>(&addr),sizeof(addr));
    if (ret < 0)
    {
        LOG_FATAL << "sockets: bind error.";
    }
}

void listenOrDie(int sockfd)
{
    int ret = ::listen(sockfd,SOMAXCONN);
    if (ret < 0)
    {
        LOG_FATAL << "sockets: listen error.";
    }
}

int accept(int sockfd, struct sockaddr_in *addr)
{
    socklen_t addrlen = sizeof(*addr);
    return ::accept(sockfd, reinterpret_cast<struct sockaddr*>(addr),&addrlen);
}
void closeOrDie(int sockfd)
{
    int ret = ::close(sockfd);
    if (ret < 0)
    {
        LOG_FATAL << "sockets: close error.";
    }
}
void shutdownWriteOrDie(int sockfd)
{
    if(::shutdown(sockfd, SHUT_WR))
    {
        LOG_FATAL << "sockets: shutdownWrite error.";
    }
}
} // namespace socket

} // namespace feipu