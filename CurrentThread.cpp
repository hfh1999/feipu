#include "CurrentThread.h"
namespace feipu {
namespace CurrentThread {
__thread pid_t cached_tid = 0; // 缓存，避免每次都进行系统调用
std::string get_tid_string() {
  char buffer[7];
  ::snprintf(buffer, 7, "%-6d", get_tid());
  return std::string(buffer);
}
pid_t get_tid() {
  if (cached_tid == 0) {
    cached_tid = static_cast<pid_t>(::syscall(SYS_gettid));
  }
  return cached_tid;
}
} // namespace CurrentThread
} // namespace feipu