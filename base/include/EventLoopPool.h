#ifndef EVENTLOOPPOOL
#define EVENTLOOPPOOL
#include "tools.h"
#include <vector>
#include "FeiTypes.h"
#include <functional>
namespace feipu {
class EventLoop;
class EventLoopThread;
class EventLoopPool : noncopyable {
public:
  typedef std::function<void(EventLoop *)> ThreadInitCallback;
  EventLoopPool(EventLoop *loop);
  ~EventLoopPool();//FIXME 由于EventloopThread的原因EventLoopPool不可以销毁太快，否则卡死
  void setNum(unsigned short loop_num);
  void start(const ThreadInitCallback& cb = ThreadInitCallback());
  EventLoop *getNextLoop();

private:
  EventLoop *main_loop_;
  std::vector<std::unique_ptr<EventLoopThread>> pool_;
  std::vector<EventLoop *> loops_;
  int loop_num_;
  int count_;
};
} // namespace feipu
#endif