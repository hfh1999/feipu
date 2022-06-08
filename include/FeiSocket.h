/*
 * 所有与网络相关的接口放置于这个文件之中，想要进行网络操作的只需引用
 * 这个文件即可。
 */
#ifndef FEISOCKET_H
#define FEISOCKET_H
#include "FeiTypes.h"
#include <arpa/inet.h>
namespace feipu {
namespace socket {
uint32_t hostToNetwork32(uint32_t);
uint16_t hostToNetwork16(uint16_t);
uint32_t networkToHost32(uint32_t);
uint16_t networkToHost16(uint16_t);

int createNonBlockingOrDie(); // 创建一个socket,非阻塞
int connect(int sockfd,const struct sockaddr_in& addr);
void bindOrDie(int sockfd,const struct sockaddr_in& addr);
void listenOrDie(int sockfd);
int accept(int sockfd,struct sockaddr_in* addr);
void closeOrDie(int sockfd);
void shutdownWriteOrDie(int sockfd);
} // namespace socket
} // namespace feipu
#endif