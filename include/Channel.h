#ifndef CHANNEL_H
#define CHANNEL_H
#include <cassert>
#include <cstdio>
#include <functional>
#include <poll.h>
#include <sys/poll.h>
#include "FeiTypes.h"
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
  typedef function<void(void)> CloseCallBack;

  //fd必须是有效打开状态的
  Channel(int fd, Eventloop *loop)
      : readcall_(), writecall_(), fd_(fd), events_(0), loop_(loop),
        is_reading_(false), is_writing_(false) {}
  int fd() { return fd_; }
  int index() { return index_; }
  void setIndex(int index) { index_ = index; }
  void setReadCall(const ReadCallBack& cb) { readcall_ = cb; }
  void enableRead() {
    assert(readcall_);
    events_ |= ReadEvent;
    is_reading_ = true;
    update_channel();
  }
  bool isReading(){return is_reading_;}
  void disableRead() {
    events_ &= ~ReadEvent;
    is_reading_ = false;
    update_channel();
  }
  void setWriteCall(const WriteCallBack& cb) { writecall_ = cb; }
  bool isWriting(){return is_writing_;}
  void enableWrite() {
    assert(writecall_);
    events_ |= WriteEvent;
    is_writing_ = true;
    update_channel();
  }
  void disableWrite() {
    events_ &= ~WriteEvent;
    is_writing_ = false;
    //un_register(); // FIXME bug here.
    update_channel();
  }
  void setCloseCall(const CloseCallBack& cb){
    closecall_ = cb;
  }
  void handleEvent();
  short events() { return events_; }
  void set_revents(int revent) { revents_ = revent; }// 给poller用
  void un_register();// 解注册
  Eventloop *getloop() { return loop_; }

private:
  ReadCallBack readcall_;
  WriteCallBack writecall_;
  CloseCallBack closecall_;
  int fd_;
  int index_; // 记录在fds中的index便于删除
  short events_;
  short revents_;
  Eventloop *loop_;
  bool is_reading_;
  bool is_writing_;

  /*这里是为了防止在同一个eventhandle里面不小心弄丢
    channel的拥有者(share_ptr管理) 
  */
  std::weak_ptr<void> tie_;

  void update_channel();// 注册或者更新
};
} // namespace feipu
#endif