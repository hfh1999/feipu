#include "Thread.h"
#include <cassert>
namespace feipu {
  void Thread::join()
  {
      thread_.join();
  }
}