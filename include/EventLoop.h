#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include "CallBack.h"
#include <cassert>
#include <map>
#include <memory>
#include <poll.h>
#include <sys/poll.h>
#include <vector>
namespace feipu {
using std::map;
using std::vector;
class Channel;
class TimerQueue;
class Eventloop {
public:
  Eventloop();
  ~Eventloop();
  void loop();                    // 一个循环
  void update_channel(Channel *); // 注册一个回调
  void removeChannel(Channel *);  // 移除一个回调
  // double 的interval的单位为秒
  void RunEvery(double interval, TimerCallback cb);
  bool isInLoopThread() const;
  void assertInLoopThread();

private:
  void updateChannelHelper(Channel *);
  void removeChannelHelper(Channel *);
  bool looping_;
  // 每个pollfds对应一个channel
  vector<pollfd> pollfds_;
  map<int, Channel *> fd_channel_map_;
  std::unique_ptr<TimerQueue> timer_queue_;

  pid_t tid_;
};
} // namespace feipu
#endif