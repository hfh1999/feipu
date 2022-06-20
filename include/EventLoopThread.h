#ifndef EVENTLOOPOTHREAD
#define EVENTLOOPOTHREAD
#include "tools.h"
#include "Thread.h"
#include "FeiTypes.h"
#include "Mutex.h"
#include "Condition.h"
namespace feipu {
class EventLoop;
class EventLoopThread : noncopyable {
public:
  EventLoopThread();
  EventLoop* getLoop();
private:
    void threadFunc_();
    Thread loopthread_;
    EventLoop* loop_;
    MutexLock mutex_;// 保证创建线程后立即调用getLoop()的正确性
    Condition cv_;
};
}
#endif