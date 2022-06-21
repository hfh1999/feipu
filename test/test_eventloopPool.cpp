#include "CurrentThread.h"
#include "EventLoop.h"
#include "EventLoopPool.h"
#include "Logging.h"
#include <unistd.h>
using namespace feipu;
void print(EventLoop *p = nullptr) {
  printf("main(): pid = %d, tid = %d, loop = %p\n", getpid(),
         CurrentThread::get_tid(), p);
}
void init(EventLoop *p) {
  printf("init(): pid = %d, tid = %d, loop = %p\n", getpid(),
         CurrentThread::get_tid(), p);
}
int main() {
  print();
  EventLoop loop;
  loop.RunAfter(5,std::bind(&EventLoop::quit,&loop));
  {
    printf("single thread %p : \n", &loop);
    EventLoopPool model(&loop);
    model.setNum(0);
    model.start(init);
    assert(model.getNextLoop() == &loop);
    assert(model.getNextLoop() == &loop);
    assert(model.getNextLoop() == &loop);
    assert(model.getNextLoop() == &loop);
    LOG_INFO << "test1 pass.";
  }

  {
    printf("Another thread %p : \n", &loop);
    EventLoopPool model(&loop);
    model.setNum(1);
    model.start(init);
    EventLoop *nextloop = model.getNextLoop();
    assert(nextloop != &loop);
    assert(nextloop == model.getNextLoop());
    assert(nextloop == model.getNextLoop());
    assert(nextloop == model.getNextLoop());
    LOG_INFO << "test2 pass.";
    ::sleep(1);
  }

  {
    printf("Three thread %p : \n", &loop);
    EventLoopPool model(&loop);
    model.setNum(3);
    model.start(init);
    EventLoop *nextloop = model.getNextLoop();
    assert(nextloop != &loop);

    assert(nextloop != model.getNextLoop());
    assert(nextloop != model.getNextLoop());
    assert(nextloop == model.getNextLoop());

    LOG_INFO << "test3 pass.";
    ::sleep(1);
  }
  LOG_INFO << "ALL pass.";
  loop.loop();
}