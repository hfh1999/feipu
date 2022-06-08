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
  typedef std::thread::id ThreadId;
  Thread(ThreadFunc func)
      : thread_(std::bind(&Thread::wrapper, this, func)), tid_(0), mutex_(),
        cv_() {
    {
      {
        UniqueLock ulock(mutex_);
        while (count == 0)
          cv_.wait(ulock);
      }
    }
  }

  pid_t get_tid() { return tid_; }
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
  pid_t tid_;
  // MutexLock mutex_; // 为了解决调用在初始时调用get_tid为0的问题
  // Condition cv_;    // 同样
  MutexLock mutex_;
  Condition cv_;
  int count = 0;
};

} // namespace feipu
#endif