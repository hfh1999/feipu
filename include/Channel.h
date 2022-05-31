#ifndef CHANNEL_H
#define CHANNEL_H
#include <cstdio>
#include <functional>
#include <poll.h>
#include <sys/poll.h>
#include <cassert>
using std::function;
typedef function<void(void)> ReadCallBack;
typedef function<void(void)> WriteCallBack;
namespace feipu{
    // Channel 保存调用，在回调时根据poller的返回值来确定是什么回调
    const int NoneEvent = 0;
    const int ReadEvent = POLLIN | POLLPRI;
    const int WriteEvent = POLLOUT;
    class Eventloop;
    class Channel{
        public:
            Channel(int fd,Eventloop* loop):
            readcall_(),
            writecall_(),
            fd_(fd),
            events_(0),
            loop_(loop)
            {}
            int fd(){return fd_;}
            void setReadCall(ReadCallBack cb){readcall_ = cb;}
            void enableRead(){
                assert(readcall_);
                events_ |= ReadEvent;
                register_loop();
            }
            void setWriteCall(WriteCallBack cb){writecall_ = cb;}
            void enableWrite(){
                assert(writecall_);
                events_ |= WriteEvent;
                register_loop();
            }
            void handleEvent(){
                if(revents_ & POLLNVAL)
                {
                    ::printf("fd is invalid.\n");
                }
                if(revents_ &(POLLIN | POLLPRI | POLLRDHUP))
                {
                    ::printf("readcall happen.\n");
                    readcall_();
                }
            }
            short events(){return events_;}
            void set_revents(int revent){revents_ = revent;}
        private:
            ReadCallBack readcall_;
            WriteCallBack writecall_;
            int fd_;
            short events_;
            short revents_;
            Eventloop* loop_;

            void register_loop();
    };
}
#endif