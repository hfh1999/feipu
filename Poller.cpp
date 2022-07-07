#include "Poller.h"
#include "Channel.h"
#include "Logging.h"
#include <cassert>
namespace feipu {
Poller::Poller()
:epollfd_(::epoll_create1(EPOLL_CLOEXEC)) // 当exec时将关闭这个文件描述符
,rec_events_(INIT_EVENTS_NUM) // 初始时rec_events为INIT_EVENTS_NUM个,动态扩充
{
}
vector<Channel *> Poller::poll(int64_t delay) {
  int eventnum = ::epoll_wait(epollfd_,&*rec_events_.begin(),rec_events_.size(),delay);
  vector<Channel *> active_channels;
  for(int i = 0; i < eventnum;i++)
  {
    Channel* channel = static_cast<Channel*>(rec_events_[i].data.ptr);
    int tmp_fd = channel->fd(); // FIXME channel为无效指针怎么办?
    auto it = fd_channel_map_.find(tmp_fd);
    assert(it != fd_channel_map_.end());
    assert(it->second == channel);
    channel->set_revents(rec_events_[i].events);
    active_channels.push_back(channel);
  }
  return active_channels;
}
//新channel会add，旧channel会update
void Poller::update_channel(Channel *in_channel) {
// 这里有种情况,即map中存在channel但是这个channel已经无任何事件设置
// 可通过设置fd为负值来让poll跳过这个pollfd
  if (fd_channel_map_.find(in_channel->fd()) != fd_channel_map_.end()) {
    // 找到了，说明是旧channel更新状态
    LOG_TRACE << "update channel.";
    assert(fd_channel_map_[in_channel->fd()]->fd() ==
           in_channel->fd()); // fd 不可能变动
    if (in_channel->isReading() && in_channel->isWriting()) {
      LOG_TRACE << "update channel,is writing and reading.";
    }
    //pollfds_[fd_channel_map_[in_channel->fd()]->index()] = tmppoll_fd;
    struct epoll_event epoll_ev;
    epoll_ev.events = in_channel->events(); // FIXME 这样兼容吗
    epoll_ev.data.ptr = static_cast<void*>(in_channel);
    ::epoll_ctl(epollfd_,EPOLL_CTL_MOD,in_channel->fd(),&epoll_ev);
    return;
  }
  fd_channel_map_[in_channel->fd()] = in_channel;
  struct epoll_event epoll_ev;
  epoll_ev.events = in_channel->events(); // FIXME 这样兼容吗
  epoll_ev.data.ptr = static_cast<void*>(in_channel);
  ::epoll_ctl(epollfd_,EPOLL_CTL_ADD,in_channel->fd(),&epoll_ev);
}
//保证不会重复remove
void Poller::removeChannel(Channel* channel_removed)
{
  LOG_TRACE << "RemoveChannel---";
  if (fd_channel_map_.find(channel_removed->fd()) == fd_channel_map_.end())
    return;
  LOG_TRACE << "RemoveChannel+++";
  assert(channel_removed == fd_channel_map_[channel_removed->fd()]);
  struct epoll_event epoll_ev;
  epoll_ev.events = channel_removed->events(); // FIXME 这样兼容吗
  epoll_ev.data.ptr = static_cast<void*>(channel_removed);
  epoll_ctl(epollfd_,EPOLL_CTL_DEL,channel_removed->fd(),&epoll_ev);
  fd_channel_map_.erase(channel_removed->fd());
}
size_t Poller::debug_ret_map_size() { return fd_channel_map_.size(); }
} // namespace feipu