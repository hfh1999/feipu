#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H
#include <thread>
namespace feipu {
namespace CurrentThread {
typedef std::thread::id ThreadId;
ThreadId get_id() { return std::this_thread::get_id(); }
} // namespace CurrentThread
} // namespace feipu
#endif