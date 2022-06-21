#ifndef EVENTLOOPOTHREAD
#define EVENTLOOPOTHREAD
#include "Thread.h"
#include "tools.h"
//#include <thread>
#include "FeiTypes.h"
//#include "Mutex.h"
//#include "Condition.h"
#include <condition_variable>
#include <functional>
#include <mutex>
namespace feipu {
class EventLoop;
class EventLoopThread : noncopyable {
public:
  typedef std::function<void(EventLoop *)> ThreadInitCallback;
  EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback());
  ~EventLoopThread();
  EventLoop *startloop();

private:
  void threadFunc_();
  Thread loopthread_;
  EventLoop *loop_;
  std::mutex mutex_; // 保证创建线程后立即调用getLoop()的正确性
  std::condition_variable cv_;
  ThreadInitCallback init_cb_;
  bool flag = false;
};
} // namespace feipu
#endif