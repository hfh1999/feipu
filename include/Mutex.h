#ifndef MUTEX_H
#define MUTEX_H
#include "CurrentThread.h"
#include "tools.h"
#include <cassert>
#include <mutex>
namespace feipu {
/*以下简单包装了标准库中的mutex*/
class MutexLock : noncopyable {
public:
  MutexLock() : mutex_(), id_holder_() {}

  void lock() {
    mutex_.lock();
    id_holder_ = CurrentThread::get_id();
  }
  void unlock() {
    id_holder_ = CurrentThread::ThreadId();
    mutex_.unlock();
  }
  bool isLockByThisThread() const {
    return id_holder_ == CurrentThread::get_id();
  }
  void assertLocked() const { assert(isLockByThisThread()); }

private:
  friend class MutexLockGuard;
  friend class UniqueLock;
  std::mutex mutex_;
  CurrentThread::ThreadId id_holder_;
};

/*以下简单的包装了lockguard*/
class MutexLockGuard:noncopyable
{
    public:
    explicit MutexLockGuard(MutexLock& mutex)
    :guard_(mutex.mutex_)
    {}
    private:
    std::lock_guard<std::mutex> guard_;
};

/*包装了标准库中的std::unique_lock*/
class UniqueLock:noncopyable
{
  public:
  explicit UniqueLock(MutexLock& mutex)
  :unique_lock_(mutex.mutex_)
  {}
  public:
  void lock(){
    unique_lock_.lock();
  }
  void unlock(){
    unique_lock_.unlock();
  }
  private:
  friend class Condition;
  std::unique_lock<std::mutex> unique_lock_;
};
} // namespace feipu
#endif