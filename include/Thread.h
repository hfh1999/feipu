#ifndef THREAD_H
#define THREAD_H
#include "tools.h"
#include <functional>
#include <thread>
/*简单起见只是将标准库中的thread加以包装*/
namespace feipu {

class Thread : noncopyable {
public:
  typedef std::function<void()> ThreadFunc;
  typedef std::thread::id ThreadId;
  Thread(ThreadFunc func) : thread_(func) {}

  ThreadId get_id() const;
  void join();


private:
  std::thread thread_;
};

} // namespace feipu
#endif