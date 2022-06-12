#include "Channel.h"
#include "EventLoop.h"
namespace feipu {

void Channel::update_channel() { loop_->update_channel(this); }
} // namespace feipu