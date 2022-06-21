#include "EventLoopThread.h"
#include "EventLoop.h"
namespace feipu {
/*有线程不安全的迹象，不可以将this在构造函数中传递给其他的线程*/
EventLoopThread::EventLoopThread(const ThreadInitCallback &cb)
    : loopthread_(),
      loop_(nullptr), init_cb_(cb) {}
EventLoopThread::~EventLoopThread() {
 
  //FIXME  若EventLoopThread销毁得过快时将会在此时卡死
  if(loop_ != nullptr)
  {
    loop_->quit();// 必须等到loop已经在运行才可以操作
    loopthread_.join();
  }
}
void EventLoopThread::threadFunc_() {
  EventLoop loop;
  if (init_cb_) {
    init_cb_(&loop);
  }
  {
    // std::unique_lock<std::mutex> guard(mutex_);
    //std::lock_guard<std::mutex> guard(mutex_);
    MutexLockGuard guard(mutex_);
    loop_ = &loop;
    cv_.notify();
  }

  flag = true;
  loop.loop();
  // 销毁的处理。。。
}
EventLoop *EventLoopThread::startloop() {
  loopthread_ = Thread(
      std::bind(&EventLoopThread::threadFunc_, this)); // 编写了移动赋值运算符
    loopthread_.start();
  {
    UniqueLock guard(mutex_);
    while (loop_ == nullptr) 
    {
      cv_.wait(guard);
    }
  }
  return loop_; // 当loop_不为nullptr时才返回
}
} // namespace feipu