#ifndef POLLER_H
#define POLLER_H
#include "tools.h"
#include "FeiTypes.h"
#include <poll.h>
#include <sys/epoll.h>
#include <vector>
#include <map>
namespace feipu {
class Channel;
class Poller:noncopyable
{
    public:
        Poller();
        std::vector<Channel*> poll(int64_t delay);
        void update_channel(Channel *in_channel);
        void removeChannel(Channel *channel_removed);
        size_t debug_ret_map_size();
        
    private:
      const int INIT_EVENTS_NUM = 16;
      std::map<int,Channel*> fd_channel_map_;// for debug
      int epollfd_;
      std::vector<epoll_event> rec_events_;// 动态扩充的数组，方便epoll_wait的返回
};
}
#endif