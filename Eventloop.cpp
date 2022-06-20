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
const int KPollIntervalMs = 10000; // 使用微秒做单位
namespace feipu {
/*忽略SIGPIPE信号*/
  class IgnoreSigPipe
{
 public:
  IgnoreSigPipe()
  {
    ::signal(SIGPIPE, SIG_IGN);
    LOG_TRACE << "Ignore SIGPIPE";
  }
};
IgnoreSigPipe initobj;
 

__thread Eventloop *loopInThisThread = 0; // 若线程中已经有了loop则不为0
Eventloop::Eventloop()
    : looping_(false), timer_queue_(new TimerQueue()),
      tid_(CurrentThread::get_tid()), wakeupFd_(createEventfd()),
      wakeChannel_(new Channel(wakeupFd_, this)),
      isDoFunctions_(false) {
  if (loopInThisThread == 0) {
    loopInThisThread = this;
  } else {
    LOG_FATAL << "Another Eventloop has exist.";
  }
  wakeChannel_->setReadCall(std::bind(&Eventloop::handleWakeEvent, this));
  wakeChannel_->enableRead();
}
int Eventloop::createEventfd() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    LOG_FATAL << "Failed in ::eventfd()";
  }
  return evtfd;
}
void Eventloop::handleWakeEvent() {
  uint64_t one = 1;
  ssize_t n = ::read(wakeupFd_, &one, sizeof(one));
  if (n != sizeof(one)) {
    LOG_ERROR << "EventLoop: wakeup() reads" << n << "bytes instead of 8";
  }
}
void Eventloop::wakeup() {
  uint64_t one = 1;
  ssize_t n = ::write(wakeupFd_, &one, sizeof one);
  if (n != sizeof one) {
    LOG_ERROR << "EventLoop::wakeup() write " << n << " bytes instead of 8";
  }
}
Eventloop::~Eventloop() { ::close(wakeupFd_); }

void Eventloop::loop() {
  looping_ = true;
  while (looping_) {
    // poll here.
    // poll 若数据没能即使取走则一直保持原来的状态

    // 获取最近超时的定时器任务的时间
    int64_t time_out = timer_queue_->getNextTimerTimerOut();
    if (time_out <= -1) {
    } else {
      time_out = time_out / 1000;
    }

    // poll 的计时不准(以ms)考虑更精准的epoll
    int eventnum = ::poll(&*pollfds_.begin(), pollfds_.size(), time_out);
    vector<Channel *> active_channels;
    for (auto item : pollfds_) {
      if (eventnum <= 0)
        break;
      if (item.revents > 0) {
        eventnum -= 1;
        Channel *channel = fd_channel_map_[item.fd];
        channel->set_revents(item.revents);
        active_channels.push_back(fd_channel_map_[item.fd]);
      }
    }

    // 处理普通的io任务
    for (auto item : active_channels) {
      item->handleEvent();
    }
    LOG_TRACE << "channel size = " << fd_channel_map_.size();

    // 处理定时器任务
    timer_queue_->handleTimeEvent();
    doFunctions();
  }
}
void Eventloop::update_channel(Channel *in_channel) {
  assert(in_channel->getloop() == this);
  assertInLoopThread(); // 确保这一函数只能在它的io线程内调用
                        // 因为要确保每个channel都是完全属于某个io线程的
  updateChannelHelper(in_channel);
}
void Eventloop::removeChannel(Channel *in_channel) {
  assert(in_channel->getloop() == this);
  assertInLoopThread(); // 同样

  removeChannelHelper(in_channel);
}

/*重复添加是可以的*/
void Eventloop::updateChannelHelper(Channel *in_channel) {
  if (fd_channel_map_.find(in_channel->fd()) != fd_channel_map_.end()) {
    // 找到了，说明是旧channel更新
    LOG_TRACE << "update channel.";
    assert(fd_channel_map_[in_channel->fd()]->fd() ==
           in_channel->fd()); // fd 不可能变动
    struct pollfd tmppoll_fd;
    tmppoll_fd.fd = in_channel->fd();
    tmppoll_fd.events = in_channel->events();
    if (in_channel->isReading() && in_channel->isWriting()) {
      LOG_TRACE << "update channel,is writing and reading.";
    }
    pollfds_[fd_channel_map_[in_channel->fd()]->index()] = tmppoll_fd;
    return;
  }
  fd_channel_map_[in_channel->fd()] = in_channel;
  struct pollfd tmppoll_fd;
  tmppoll_fd.fd = in_channel->fd();
  tmppoll_fd.events = in_channel->events();
  pollfds_.push_back(std::move(tmppoll_fd));
  in_channel->setIndex(pollfds_.size() - 1);
}

/*重复删除也没问题*/
void Eventloop::removeChannelHelper(Channel *in_channel) {
  LOG_TRACE << "RemoveChannel---";
  if (fd_channel_map_.find(in_channel->fd()) == fd_channel_map_.end())
    return;
  LOG_TRACE << "RemoveChannel+++";
  assert(in_channel == fd_channel_map_[in_channel->fd()]);
  fd_channel_map_.erase(in_channel->fd());

  // 交换最后一个和待删除的一个，然后删除最后一个即可，保证效率
  std::swap(pollfds_[pollfds_.size() - 1], pollfds_[in_channel->index()]);
  pollfds_.erase(pollfds_.end() - 1);
}
void Eventloop::runInLoop(const Functor &cb) {
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
void Eventloop::doFunctions() {
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
void Eventloop::RunEvery(double interval, TimerCallback cb) {

  Timer *new_timer =
      new Timer(cb, addTime(TimeStamp::now(), interval), interval);
  timer_queue_->addTimerInLoop(new_timer);
}
bool Eventloop::isInLoopThread() const {
  return tid_ == CurrentThread::get_tid();
}
void Eventloop::assertInLoopThread() {
  if (!isInLoopThread()) {
    LOG_FATAL << "threadId=" << tid_;
  }
}
} // namespace feipu