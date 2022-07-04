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
        Poller() = default;
        std::vector<Channel*> poll();
        
    private:
      std::vector<pollfd> pollfds_;
      std::map<int,Channel*> fd_channel_map_;
};
}
#endif