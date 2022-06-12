#ifndef CHANNEL_H
#define CHANNEL_H
#include <cassert>
#include <cstdio>
#include <functional>
#include <poll.h>
#include <sys/poll.h>
using std::function;
namespace feipu {
// Channel 保存调用，在回调时根据poller的返回值来确定是什么回调
const int NoneEvent = 0;
const int ReadEvent = POLLIN | POLLPRI;
const int WriteEvent = POLLOUT;
class Eventloop;
class Channel {
public:
  typedef function<void(void)> ReadCallBack;
  typedef function<void(void)> WriteCallBack;
  Channel(int fd, Eventloop *loop)
      : readcall_(), writecall_(), fd_(fd), events_(0), loop_(loop) {}
  int fd() { return fd_; }
  int index() { return index_; }
  void setIndex(int index) { index_ = index; }
  void setReadCall(ReadCallBack cb) { readcall_ = cb; }
  void enableRead() {
    assert(readcall_);
    events_ |= ReadEvent;
    update_channel();
  }
  void disableRead() {
    events_ &= ~ReadEvent;
    update_channel();
  }
  void setWriteCall(WriteCallBack cb) { writecall_ = cb; }
  void enableWrite() {
    assert(writecall_);
    events_ |= WriteEvent;
    update_channel();
  }
  void disableWrite() {
    events_ &= ~WriteEvent;
    update_channel();
  }
  void handleEvent() {
    if (revents_ & POLLNVAL) {
      ::printf("fd is invalid.\n");
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
      //::printf("readcall happen.\n");
      readcall_();
    }
  }
  short events() { return events_; }
  void set_revents(int revent) { revents_ = revent; }
  Eventloop *getloop() { return loop_; }

private:
  ReadCallBack readcall_;
  WriteCallBack writecall_;
  int fd_;
  int index_; // 记录在fds中的index便于删除
  short events_;
  short revents_;
  Eventloop *loop_;

  void update_channel();
};
} // namespace feipu
#endif