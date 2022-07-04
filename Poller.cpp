#include "Poller.h"
#include "Channel.h"
#include "Logging.h"
#include <cassert>
namespace feipu {
Poller::Poller() = default;
vector<Channel *> Poller::poll(int64_t delay) {
  delay = delay / 1000; // poll 精度为ms
  int eventnum = ::poll(&*pollfds_.begin(), pollfds_.size(), delay);
  vector<Channel *> active_channels;
  for (auto item : pollfds_) {
    if (eventnum <= 0)
      break;
    if (item.revents > 0) // 有事件发生
    {
      assert(fd_channel_map_.find(item.fd) != fd_channel_map_.end());
      Channel *channel = fd_channel_map_[item.fd];
      //填写channel
      channel->set_revents(item.revents);
      active_channels.push_back(channel);
      eventnum -= 1;
    }
  }
  return active_channels;
}
void Poller::update_channel(Channel *in_channel) {
// 这里有种情况,即map中存在channel但是这个channel已经无任何事件设置
// 可通过设置fd为负值来让poll跳过这个pollfd
  if (fd_channel_map_.find(in_channel->fd()) != fd_channel_map_.end()) {
    // 找到了，说明是旧channel更新状态
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
void Poller::removeChannel(Channel* channel_removed)
{
  LOG_TRACE << "RemoveChannel---";
  if (fd_channel_map_.find(channel_removed->fd()) == fd_channel_map_.end())
    return;
  LOG_TRACE << "RemoveChannel+++";
  assert(channel_removed == fd_channel_map_[channel_removed->fd()]);
  int to_erase_index = channel_removed->index();
  fd_channel_map_.erase(channel_removed->fd());

  // 交换最后一个和待删除的一个，然后删除最后一个即可，保证效率
  std::swap(pollfds_[pollfds_.size() - 1], pollfds_[channel_removed->index()]);
  // 原来的最后一个需要更新其index
  fd_channel_map_[pollfds_[to_erase_index].fd]->setIndex(to_erase_index);
  pollfds_.erase(pollfds_.end() - 1);
}
size_t Poller::debug_ret_map_size() { return fd_channel_map_.size(); }
} // namespace feipu