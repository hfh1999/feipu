#include "Channel.h"
#include "EventLoop.h"
namespace feipu {

    void Channel::register_loop(){
        loop_->addChannel(this);
    }
}