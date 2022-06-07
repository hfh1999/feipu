#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H
#include <cstdio>
#include <string>
#include <thread>
namespace feipu {
namespace CurrentThread {
typedef std::thread::id ThreadId;
ThreadId get_id() { return std::this_thread::get_id(); }
std::string get_id_string() {
  char buffer[7];
  ::snprintf(buffer, 7, "%-6d", 0);
  return std::string(buffer);
}
} // namespace CurrentThread
} // namespace feipu
#endif