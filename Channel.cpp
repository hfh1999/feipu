#include "Channel.h"
#include "EventLoop.h"
#include "Logging.h"
namespace feipu {

void Channel::update_channel() { loop_->update_channel(this); }
void Channel::un_register() { loop_->removeChannel(this); }
void Channel::handleEvent() {
  std::shared_ptr<void> guard;

  if(is_tied)
  {
    assert(!tie_.expired()); // tie_ 应该在这里始终有效
    guard = tie_.lock();     // 抓住，防止channel的所有者丢失
  }

  if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) { // 不知道有何用
    LOG_WARN << "Channel::handle_event() POLLHUP";
    if (closecall_)
      closecall_();
  }
  if (revents_ & POLLNVAL) {
    LOG_WARN << "fd is invalid.";
  }
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    readcall_();
  }
  if (revents_ & POLLOUT) {
    LOG_TRACE << "writecall.";
    writecall_();
  }
}
void Channel::tie(std::shared_ptr<void> obj)
{
  tie_ = obj;
  is_tied = true;
}
} // namespace feipu