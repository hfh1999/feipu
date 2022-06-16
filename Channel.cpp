#include "Channel.h"
#include "EventLoop.h"
namespace feipu {

void Channel::update_channel() { loop_->update_channel(this); }
void Channel::un_register(){loop_->removeChannel(this);}
} // namespace feipu