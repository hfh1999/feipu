#include "Channel.h"
#include "EventLoop.h"
#include "Logging.h"
#include "Timer.h"
#include <bits/types/__FILE.h>
#include <cstdio>
#include <fcntl.h>
#include <sys/syscall.h>
#include <syscall.h>
#include <unistd.h>
#include "Thread.h"
using feipu::Eventloop;
using feipu::Logger;
int fd;
void test_thread(feipu::Channel* channel)
{
  channel->enableRead();
  while(true)
  {
  }
}
void test_fuc() {
  char buffer[1024 * 6];
  int count = 0;
  while ((count = read(fd, buffer, sizeof(buffer))) > 0) {
    printf("read %d bytes.\n", count);
    continue;
  }
  printf("end...\n");
}
void time_test() {
  static int count = 0;
  printf("hello timer!: %d\n", count++);
}
void time_test2() {
  static int count = 0;
  printf("hello timer2: %d\n", count++);
}
void time_test3() {
  static int count = 0;
  printf("hello timer3: %d\n", count++);
}
int main() {
  Logger::setLogLevel(Logger::TRACE);
  LOG_INFO << "the start.";
  fd = ::open("/dev/pts/1", O_RDONLY | O_NONBLOCK);
  LOG_TRACE << "trace ???";
  LOG_DEBUG << "debug ???";
  // LOG_FATAL << "to abort";
  Eventloop myloop;
  myloop.RunEvery(1, time_test);
  myloop.RunEvery(5, time_test2);
  myloop.RunEvery(3, time_test3);
  feipu::Channel mychannel(fd, &myloop);
  feipu::Channel mychannel2(fd, &myloop);
  feipu::Thread t1(std::bind(test_thread,&mychannel2));
  mychannel.setReadCall(test_fuc);
  mychannel.enableRead();
  myloop.loop();
}