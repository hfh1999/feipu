#include "EventLoopPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"
#include "Thread.h"
namespace feipu {
EventLoopPool::~EventLoopPool() {
} // 必须放在这个文件中，因为unique_ptr.
EventLoopPool::EventLoopPool(EventLoop *loop)
    : main_loop_(loop), loop_num_(0), count_(0) {}
void EventLoopPool::setNum(unsigned short loop_num) {
  main_loop_->assertInLoopThread();
  loop_num_ = loop_num;
}
void EventLoopPool::start(const ThreadInitCallback &cb) {
  main_loop_->assertInLoopThread();
  if (loop_num_ == 0) {
    if (cb) {
      cb(main_loop_);
    }
    return;
  }
  for (int i = 0; i < loop_num_; i++) {
    EventLoopThread *tmp_thread_p = new EventLoopThread(cb);
    loops_.push_back(tmp_thread_p->startloop());
    pool_.push_back(std::unique_ptr<EventLoopThread>(tmp_thread_p));
  }
}
EventLoop *EventLoopPool::getNextLoop() {
  main_loop_->assertInLoopThread();
  if (loop_num_ == 0) {
    return main_loop_;
  }
  EventLoop *ret_loop;
  ret_loop = loops_[count_];
  count_ += 1;
  if (count_ == loop_num_) {
    count_ = 0;
  }
  return ret_loop;
}
} // namespace feipu