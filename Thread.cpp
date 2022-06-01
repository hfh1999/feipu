#include "Thread.h"
#include <cassert>
namespace feipu {
  Thread::ThreadId Thread::get_id() const 
  {
      return thread_.get_id();
  }
  void Thread::join()
  {
      thread_.join();
  }
}