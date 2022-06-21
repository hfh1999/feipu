#include "EventLoop.h"
#include "EventLoopThread.h"
#include <unistd.h>
using namespace feipu;
void print(EventLoop *p = NULL) {
  printf("print: pid = %d, tid = %d, loop = %p\n", getpid(),
         CurrentThread::get_tid(), p);
}
void quit(EventLoop *p) {
  print(p);
  p->quit();
}
int main() {
  print();
  {
    EventLoopThread thr2;
    EventLoop *loop = thr2.startloop();
    loop->runInLoop(std::bind(print, loop));
    CurrentThread::sleepUsec(500 * 1000); // 500ms
  }
}
