#ifndef TIMER_H
#define TIMER_H
#include "CallBack.h"
#include "Timestamp.h"
namespace feipu {
/*这个类中包含了有关定时器时间及定时调用的信息*/
class Timer {
public:
  Timer(TimerCallback cb, TimeStamp when, double interval)
      : timecall_(cb), expiration_(when), interval_(interval),
        repeat_(interval > 0) {}

    void run()
    {
        timecall_();
    }
    TimeStamp expiration()
    {
        return expiration_;
    }
    bool is_repeat()
    {
        return repeat_;
    }
    void reset_expiration(TimeStamp now)
    {
        expiration_ = addTime(now,interval_);
    }

private:
  TimerCallback timecall_;
  TimeStamp expiration_;
  double interval_;
  bool repeat_;
};
} // namespace feipu
#endif