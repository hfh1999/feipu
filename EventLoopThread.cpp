#include "EventLoopThread.h"
#include "EventLoop.h"
namespace feipu {
    EventLoopThread::EventLoopThread():
    loopthread_(std::bind(&EventLoopThread::threadFunc_,this)),
    loop_(nullptr)
    {}
    void EventLoopThread::threadFunc_()
    {
        EventLoop loop;

        {
          UniqueLock guard(mutex_);
          loop_ = &loop;
          cv_.notify();
        }

        loop.loop();
    }
    EventLoop* EventLoopThread::getLoop()
    {
        {
            UniqueLock guard(mutex_);
            while(loop_ == nullptr)
            {
                cv_.wait(guard);
            }
        }
        return loop_; // 当loop_不为nullptr时才返回
    }
}