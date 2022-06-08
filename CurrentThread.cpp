#include "CurrentThread.h"
#include <unistd.h>
#include <sys/syscall.h>
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
} // namespace CurrentThread
} // namespace feipu