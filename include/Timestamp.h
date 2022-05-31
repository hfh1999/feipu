#ifndef TIMESTAMP_H
#define TIMESTAMP_H
#include <bits/stdint-intn.h>
#include <bits/types/struct_timeval.h>
#include <cstdint>
#include <sys/time.h>
/*我们在库中使用TimeStamp来表示绝对时间*/
namespace feipu{
class TimeStamp {
public:
  TimeStamp() : microSecondsSinceEpoch_(0) {}
  explicit TimeStamp(int64_t microSecondsFromEpoch)
      : microSecondsSinceEpoch_(microSecondsFromEpoch) {}
  int64_t GetmicroSecondsSinceEpoch() { return microSecondsSinceEpoch_; }
  static TimeStamp now();
  static const int MicroSecondPerSecond = 1000 * 1000;
private:
  int64_t microSecondsSinceEpoch_;
};
inline TimeStamp TimeStamp::now(){
  struct timeval tv;
  ::gettimeofday(&tv,nullptr);
  int64_t seconds = tv.tv_sec;
  return TimeStamp(seconds * MicroSecondPerSecond + tv.tv_usec);
}
inline int64_t timeDifference(TimeStamp high,TimeStamp low)
{
  return high.GetmicroSecondsSinceEpoch() - low.GetmicroSecondsSinceEpoch();
}
// 以秒为单位的addTime
inline TimeStamp addTime(TimeStamp timestamp,double interval)
{
  int64_t delta = static_cast<int64_t>(interval * TimeStamp::MicroSecondPerSecond);
  return TimeStamp(timestamp.GetmicroSecondsSinceEpoch() + delta);
}
// 以微秒为单位的addTime
inline TimeStamp addTime(TimeStamp timestamp,int64_t interval)
{
  return TimeStamp(timestamp.GetmicroSecondsSinceEpoch() + interval);
}
inline bool operator <(TimeStamp lhs,TimeStamp rhs)
{
  return lhs.GetmicroSecondsSinceEpoch() < rhs.GetmicroSecondsSinceEpoch();
}
}
#endif