#ifndef EVENTLOOPPOOL
#define EVENTLOOPPOOL
#include "tools.h"
#include <vector>
namespace feipu {
class EventLoop;
class EventLoopThread;
class EventLoopPool : noncopyable {
public:
  EventLoopPool(EventLoop *loop);
  void setNum(unsigned short loop_num);
  void start();
  EventLoop *getNextLoop();

private:
  EventLoop *main_loop_;
  std::vector<EventLoopThread> pool_;
  std::vector<EventLoop *> loops_;
  int loop_num_;
  int count_;
};
} // namespace feipu
#endif