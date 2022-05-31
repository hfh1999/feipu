#include "EventLoop.h"
#include "Channel.h"
#include "TimerQueue.h"
#include "Timestamp.h"
#include <poll.h>
#include <sys/poll.h>
const int KPollIntervalMs = 10000; // 使用微秒做单位
namespace feipu {
Eventloop::Eventloop() : looping_(false), timer_queue_(new TimerQueue()) {}
Eventloop::~Eventloop(){}

void Eventloop::loop() {
  looping_ = true;
  while (looping_) {
    // poll here.
    // poll 若数据没能即使取走则一直保持原来的状态

    // 获取最近超时的定时器任务的时间
    int64_t time_out = timer_queue_->getNextTimerTimerOut();
    if(time_out <= -1)
    {}
    else {
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

    // 处理定时器任务
    timer_queue_->handleTimeEvent();
  }
}
void Eventloop::addChannel(Channel *in_channel) {
  fd_channel_map_[in_channel->fd()] = in_channel;
  struct pollfd tmppoll_fd;
  tmppoll_fd.fd = in_channel->fd();
  tmppoll_fd.events = in_channel->events();
  pollfds_.push_back(std::move(tmppoll_fd));
}
void Eventloop::RunEvery(double interval, TimerCallback cb){

  Timer * new_timer = new Timer(cb,addTime(TimeStamp::now(),interval),interval);
  timer_queue_->addTimerInLoop(new_timer);
}
} // namespace feipu