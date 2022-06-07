#ifndef CONDITION_H
#define CONDITION_H
#include "tools.h"
#include <condition_variable>
#include <mutex>
#include "Mutex.h"
namespace feipu {
/*对标准库condition的包装*/
class Condition : noncopyable {
public:
    Condition(UniqueLock& lock):
    lock_(lock.unique_lock_)
    {}

    void wait()
    {
        cv_.wait(lock_);
    }
    void notify(){
        cv_.notify_one();
    }
    void notifyAll(){
        cv_.notify_all();
    }
private:
    std::condition_variable cv_;
    std::unique_lock<std::mutex>& lock_;
};
} // namespace feipu
#endif