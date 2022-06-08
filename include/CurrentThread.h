#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H
#include <cstdio>
#include <sched.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <syscall.h>
namespace feipu {
namespace CurrentThread {
pid_t get_tid();
std::string get_tid_string();
} // namespace CurrentThread
} // namespace feipu
#endif