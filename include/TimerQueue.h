#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H
/* 这个类用于管理定时器 */
#include <algorithm>
#include <bits/stdint-intn.h>
#include <cstdint>
#include <iterator>
#include <set>
#include "EventLoop.h"
#include "Timer.h"
#include "Timestamp.h"
#include <vector>
namespace feipu{
    using std::vector;
class Timer;
class TimerQueue{
    public:
        /*int64_t可表示1970年的epoch的上下很多年，完全满足要求*/
        int64_t getNextTimerTimerOut(){
            /*第一个要过期的定时器距离过期的时间点的时间,以微秒计数*/
            TimeStamp nowStamp = TimeStamp::now();
            now_chache_ = nowStamp;

            // 若目前没有定时器任务
            if(timers_.size() == 0)
                return -1;  // 永久阻塞

            int64_t timeout;
            if((timeout = timeDifference(timers_.begin()->first,nowStamp)) <= 0)
            {
                return 0;
            }
            else {
                return timeout;
            }
        }
        void addTimerInLoop(Timer* in_timer){
            insert(in_timer);
        }
        void handleTimeEvent(){
            vector<Timer*> expired = getExpired();
            for(auto item: expired)
            {
                item->run();
            }

            // 这里需要加入有重复(repeat)标志的timers
            for(auto item:expired)
            {
                if(item->is_repeat())
                {
                    item->reset_expiration(now_chache_);
                    insert(item);
                }
                else {
                    // 不加入的话就直接销毁
                    delete item;
                }
            }

        }
        
        // 从timers中移除过期的timer,并以vector返回
        std::vector<Timer*> getExpired(){
            vector<Timer*> expired;
            Entry sentry(now_chache_,reinterpret_cast<Timer*>(UINTPTR_MAX));// 为了防止重复的没有被选取，因此需要一个最大值的timer指针
            std::set<Entry>::iterator end =  timers_.lower_bound(sentry);
            for(auto it = timers_.begin(); it != end; it++)
            {
                expired.push_back((*it).second);
            }
            timers_.erase(timers_.begin(),end);
            return expired;
        }

        // 向timers_中加入新的timer
        void insert(Timer* in_timer)
        {
            timers_.insert(Entry(in_timer->expiration(),in_timer));
        }

        private:
        /*todo 使用堆来管理定时器对象*/
        
        typedef std::pair<TimeStamp,Timer*> Entry;
        // 仍然暂时采用muduo的set进行管理
        std::set<Entry> timers_;
        TimeStamp now_chache_; // 用来防止在一个循环内多次调用gettimeofday
};
}
#endif