#include "EventLoop.h"
#include "Channel.h"
#include "CurrentThread.h"
#include "Logging.h"
#include "TimerQueue.h"
#include "Timestamp.h"
#include <poll.h>
#include <sys/eventfd.h>
#include <sys/poll.h>
#include <sys/signal.h>
#include "Poller.h"
const int KPollIntervalMs = 10000; // 使用微秒做单位
namespace feipu {
/*忽略SIGPIPE信号*/
class IgnoreSigPipe {
public:
  IgnoreSigPipe() {
    ::signal(SIGPIPE, SIG_IGN);
    LOG_TRACE << "Ignore SIGPIPE";
  }
};
IgnoreSigPipe initobj;

__thread EventLoop *loopInThisThread = 0; // 若线程中已经有了loop则不为0
EventLoop::EventLoop()
    : looping_(false), timer_queue_(new TimerQueue(this)),
      tid_(CurrentThread::get_tid()), wakeupFd_(createEventfd()),
      wakeChannel_(new Channel(wakeupFd_, this)), isDoFunctions_(false) {
  if (loopInThisThread == 0) {
    loopInThisThread = this;
  } else {
    LOG_FATAL << "Another Eventloop has exist.";
  }
  wakeChannel_->setReadCall(std::bind(&EventLoop::handleWakeEvent, this));
  wakeChannel_->enableRead();
}
int EventLoop::createEventfd() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    LOG_FATAL << "Failed in ::eventfd()";
  }
  return evtfd;
}
void EventLoop::handleWakeEvent() {
  uint64_t one = 1;
  ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
  if (n != sizeof(one)) {
    LOG_ERROR << "EventLoop: wakeup() reads" << n << "bytes instead of 8";
  }
}
void EventLoop::wakeup() {
  uint64_t one = 1;
  ssize_t n = ::write(wakeupFd_, &one, sizeof one);
  if (n != sizeof one) {
    LOG_ERROR << "EventLoop::wakeup() write " << n << " bytes instead of 8";
  }
}
EventLoop::~EventLoop() { ::close(wakeupFd_); }

void EventLoop::quit()
{
  looping_ = false;
  if(!isInLoopThread())
  {
    wakeup();
  }
}
void EventLoop::loop() {
  assertInLoopThread();
  looping_ = true;
  while (looping_) {
    // poll here.
    // poll 若数据没能即使取走则一直保持原来的状态

    // 获取最近超时的定时器任务的时间
    int64_t time_out = timer_queue_->getNextTimerTimerOut();
    if (time_out <= -1) {
      time_out = time_out; // 阻塞
    } else {
      time_out = time_out; // FIXME 可以去除
    }

    vector<Channel *> active_channels;
    active_channels = poller_->poll(time_out);


    // 处理普通的io任务
    for (auto item : active_channels) {
      item->handleEvent();
    }
    LOG_TRACE << "channel size = " << poller_->debug_ret_map_size();

    // 处理定时器任务
    timer_queue_->handleTimeEvent();
    doFunctions();
  }
}
void EventLoop::update_channel(Channel *in_channel) {
  /** FIXME 假若这里是在一个事件的处理过程中这里会有什么影响呢?**/
  assert(in_channel->getloop() == this);
  assertInLoopThread(); // 确保这一函数只能在它的io线程内调用
                        // 因为要确保每个channel都是完全属于某个io线程的
  poller_->update_channel(in_channel);
}
void EventLoop::removeChannel(Channel *in_channel) {
  /** FIXME 假若这里是在一个事件的处理过程中这里会有什么影响呢?**/
  assert(in_channel->getloop() == this);
  assertInLoopThread(); // 同样

  poller_->removeChannel(in_channel);
}
void EventLoop::runInLoop(const Functor &cb) {
  if (isInLoopThread()) {
    cb();
  } else {
    {
      MutexLockGuard guard(mutex_);
      functions_.push_back(cb);

      wakeup();
    }
  }
}
void EventLoop::doFunctions() {
  isDoFunctions_ = true;
  std::vector<Functor> functors;
  {
    MutexLockGuard guard(mutex_);
    functors.swap(functions_);
  }
  for (auto func : functors) {
    func();
  }
  isDoFunctions_ = false;
}
void EventLoop::RunEvery(double interval, const TimerCallback& cb) {
  TimeStamp start_time = addTime(TimeStamp::now(),interval);
  timer_queue_->addTimer(cb, start_time, interval);
}
void EventLoop::RunAfter(double delay, const TimerCallback& cb) {
  TimeStamp start_time = addTime(TimeStamp::now(),delay);
  timer_queue_->addTimer(cb, start_time, 0.0);
}
void EventLoop::RunAt(TimeStamp when, TimerCallback cb)
{
  timer_queue_->addTimer(cb, when, 0.0);
}
bool EventLoop::isInLoopThread() const {
  return tid_ == CurrentThread::get_tid();
}
void EventLoop::assertInLoopThread() {
  if (!isInLoopThread()) {
    LOG_FATAL << "threadId=" << tid_;
  }
}
} // namespace feipu