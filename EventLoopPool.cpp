#include "EventLoopPool.h"
#include "EventLoopThread.h"
#include "EventLoop.h"
#include "Thread.h"
namespace feipu {
EventLoopPool::EventLoopPool(EventLoop *loop)
    : main_loop_(loop), loop_num_(0),
    count_(0) {}
void EventLoopPool::setNum(unsigned short loop_num) {
  main_loop_->assertInLoopThread();
  loop_num_ = loop_num;
}
void EventLoopPool::start() {
  main_loop_->assertInLoopThread();
  if (loop_num_ == 0) {
    return;
  }
  for (int i = 0; i < loop_num_; i++) {
    EventLoopThread tmp_thread;
    loops_.push_back(tmp_thread.getLoop());
    pool_.push_back(std::move(tmp_thread));
  }
}
EventLoop *EventLoopPool::getNextLoop() {
  main_loop_->assertInLoopThread();
  if (loop_num_ == 0) {
    return main_loop_;
  }
  return loops_[count_];
  count_ += 1;
  if(count_ == loop_num_)
  {
    count_ = 0;
  }
}
} // namespace feipu