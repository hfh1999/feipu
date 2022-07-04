#ifndef POLLER_H
#define POLLER_H
#include "tools.h"
#include "FeiTypes.h"
#include <poll.h>
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
      std::vector<pollfd> pollfds_;
      std::map<int,Channel*> fd_channel_map_;
};
}
#endif