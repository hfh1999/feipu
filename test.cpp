#include "Channel.h"
#include "EventLoop.h"
#include "Timer.h"
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
using feipu::Eventloop;
int fd;
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
  fd = ::open("/dev/pts/1", O_RDONLY | O_NONBLOCK);
  Eventloop myloop;
  myloop.RunEvery(1, time_test);
  myloop.RunEvery(5, time_test2);
  myloop.RunEvery(3, time_test3);
  feipu::Channel mychannel(fd, &myloop);
  mychannel.setReadCall(test_fuc);
  mychannel.enableRead();
  myloop.loop();
}