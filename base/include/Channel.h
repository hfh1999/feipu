#ifndef CHANNEL_H
#define CHANNEL_H
#include "FeiTypes.h"
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
class EventLoop;
class Channel {
public:
  typedef function<void(void)> ReadCallBack;
  typedef function<void(void)> WriteCallBack;
  typedef function<void(void)> CloseCallBack;

  // fd必须是有效打开状态的
  Channel(int fd, EventLoop *loop);
  int fd() { return fd_; }
  int index() { return index_; }
  void setIndex(int index) { index_ = index; }
  // 建议使用disableAll() 而不是disableRead + disableWrite,因为其会额外错误
  void disableAll(){
    events_ = NoneEvent;
    is_reading_ = false;
    is_writing_ = false;
    update_channel();
  }
  bool isNoneEvent(){return events_ == NoneEvent;}
  void setReadCall(const ReadCallBack &cb) { readcall_ = cb; }
  void enableRead() {
    assert(readcall_);
    events_ |= ReadEvent;
    is_reading_ = true;
    update_channel();
  }
  bool isReading() { return is_reading_; }
  void disableRead() {
    events_ &= ~ReadEvent;
    is_reading_ = false;
    update_channel();
  }
  void setWriteCall(const WriteCallBack &cb) { writecall_ = cb; }
  bool isWriting() { return is_writing_; }
  void enableWrite() {
    assert(writecall_);
    events_ |= WriteEvent;
    is_writing_ = true;
    update_channel();
  }
  void disableWrite() {
    events_ &= ~WriteEvent;
    is_writing_ = false;
    update_channel();
  }
  void setCloseCall(const CloseCallBack &cb) { closecall_ = cb; }
  void handleEvent();
  short events() { return events_; }
  void set_revents(int revent) { revents_ = revent; } // 给poller用
  void un_register();                                 // 解注册
  EventLoop *getloop() { return loop_; }

  void tie(std::shared_ptr<void> obj);

private:
  ReadCallBack readcall_;
  WriteCallBack writecall_;
  CloseCallBack closecall_;
  int fd_;
  int index_; // 记录在fds中的index便于删除
  short events_;
  short revents_;
  EventLoop *loop_;
  bool is_reading_;
  bool is_writing_;

  /*这里是为了防止在同一个eventhandle里面不小心弄丢
    channel的拥有者(share_ptr管理)
  */
  std::weak_ptr<void> tie_;
  bool is_tied;

  void update_channel(); // 注册或者更新
};
} // namespace feipu
#endif