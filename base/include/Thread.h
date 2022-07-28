#ifndef THREAD_H
#define THREAD_H
#include "Condition.h"
#include "CurrentThread.h"
#include "Logging.h"
#include "Mutex.h"
#include "tools.h"
#include <functional>
#include <thread>
#include <unistd.h>
/*简单起见只是将标准库中的thread加以包装*/
namespace feipu {

class Thread : noncopyable {
public:
  typedef std::function<void()> ThreadFunc;
  /*修改错误,不可以将自身this传递给跨线程的对象。*/
  Thread(ThreadFunc func) : thread_(), func_(func), tid_(0), mutex_(), cv_() {}
  Thread() : thread_(), func_(), tid_(0), mutex_(), cv_() {}
  Thread(Thread &&t) noexcept
      : thread_(std::move(t.thread_)), func_(t.func_), tid_(t.tid_), cv_(),
        count(t.count) {}
  Thread& operator=(Thread && rhs) noexcept
  {
    if(this != &rhs)
    {
      thread_ = std::move(rhs.thread_);
      func_ = rhs.func_;
      tid_ = rhs.tid_;
      //cv_
      count = rhs.count;
    }
    return *this;
  }

  pid_t get_tid() { return tid_; }
  void start(){
    thread_ = std::thread(&Thread::wrapper,this,func_);
    {
      UniqueLock ulock(mutex_);
      while (count == 0)
      {
        cv_.wait(ulock);
      }
    }
  }
  void join();

private:
  // 这是为了获取tid之用
  void wrapper(ThreadFunc in_func) {
    tid_ = CurrentThread::get_tid();
    {
      MutexLockGuard lockguard(mutex_);
      count = 1;
      cv_.notify();
    }
    in_func();
  }
  std::thread thread_;
  ThreadFunc func_;
  pid_t tid_;
  // MutexLock mutex_; // 为了解决调用在初始时调用get_tid为0的问题
  // Condition cv_;    // 同样
  MutexLock mutex_;
  Condition cv_;
  int count = 0;
};

} // namespace feipu
#endif