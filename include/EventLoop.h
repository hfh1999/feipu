#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include "CallBack.h"
#include "FeiTypes.h"
#include "Mutex.h"
#include <cassert>
#include <map>
#include <memory>
#include <poll.h>
#include <sys/poll.h>
#include <vector>
#include "Timestamp.h"
namespace feipu {
using std::map;
using std::vector;
class Channel;
class TimerQueue;
class EventLoop {
public:
  typedef std::function<void()> Functor;
  EventLoop();
  ~EventLoop();
  void loop();                    // 一个循环
  void update_channel(Channel *); // 注册一个回调
  void removeChannel(Channel *);  // 移除一个回调
  void
  runInLoop(const Functor &cb); // 将其他线程的函数转移到这个eventloop中执行

  // double 的interval的单位为秒
  void RunEvery(double interval, const TimerCallback& cb);// 线程安全的
  void RunAfter(double delay, const TimerCallback& cb);// 线程安全的
  void RunAt(TimeStamp when, TimerCallback cb);           // 线程安全的
  bool isInLoopThread() const;
  void assertInLoopThread();
  void quit(); // 不是线程安全的。

private:
  void updateChannelHelper(Channel *);
  void removeChannelHelper(Channel *);
  void doFunctions();
  int createEventfd();
  void wakeup();
  void handleWakeEvent(); // 当wakeupfd 可读时读取事件
  bool looping_;

  MutexLock mutex_;           // 保护functions_
  vector<Functor> functions_; // 要执行的fuctions
  int wakeupFd_;
  std::shared_ptr<Channel> wakeChannel_;
  bool
      isDoFunctions_; // 在dofunctions()函数中，可能functor仍然会调用run_in_loop,这是为了控制是否要wake的标志

  vector<pollfd> pollfds_; // 每个pollfds对应一个channel
  map<int, Channel *> fd_channel_map_;
  std::unique_ptr<TimerQueue> timer_queue_;

  pid_t tid_;
};
} // namespace feipu
#endif