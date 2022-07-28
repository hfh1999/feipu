#ifndef CONDITION_H
#define CONDITION_H
#include "tools.h"
#include <condition_variable>
#include <mutex>
#include "Mutex.h"
namespace feipu {
/*对标准库condition的包装*/
// TODO 修改condition使其和标准库用法一致
class Condition : noncopyable {
public:
    Condition()
    {}

    void wait(UniqueLock& lock)
    {
        cv_.wait(lock.unique_lock_);
    }
    void notify(){
        cv_.notify_one();
    }
    void notifyAll(){
        cv_.notify_all();
    }
private:
    std::condition_variable cv_;
};
} // namespace feipu
#endif