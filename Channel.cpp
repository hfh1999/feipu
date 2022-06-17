#include "Channel.h"
#include "EventLoop.h"
#include "Logging.h"
namespace feipu {

void Channel::update_channel() { loop_->update_channel(this); }
void Channel::un_register() { loop_->removeChannel(this); }
void Channel::handleEvent() {
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
  //FIXME 若前面readcall_中发现信道关闭，则会析构TcpConnection.(连通这个channel实例也remove了)
  if (revents_ & POLLOUT) {
    LOG_TRACE << "eventloop: writecall.";
    writecall_();
  }
}
} // namespace feipu