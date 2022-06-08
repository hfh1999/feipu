#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H
#include "FeiTypes.h"
#include <unistd.h>
namespace feipu {
namespace CurrentThread {
pid_t get_tid();
string get_tid_string();
} // namespace CurrentThread
} // namespace feipu
#endif