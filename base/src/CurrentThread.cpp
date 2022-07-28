#include "CurrentThread.h"
#include <unistd.h>
#include <sys/syscall.h>
#include "Timestamp.h"
namespace feipu {
namespace CurrentThread {
__thread pid_t cached_tid = 0; // 缓存，避免每次都进行系统调用
std::string get_tid_string() {
  return string_format("%5d ",get_tid());
}
pid_t get_tid() {
  if (cached_tid == 0) {
    cached_tid = static_cast<pid_t>(::syscall(SYS_gettid));
  }
  return cached_tid;
}
void sleepUsec(int64_t usec)
{
  struct timespec ts = {0,0};
  ts.tv_sec = static_cast<time_t>(usec/TimeStamp::MicroSecondPerSecond);
  ts.tv_nsec = static_cast<time_t>(usec % TimeStamp::MicroSecondPerSecond * 1000);
  ::nanosleep(&ts, nullptr);
}
} // namespace CurrentThread
} // namespace feipu