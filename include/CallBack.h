#ifndef CALL_BACK_H
#define CALL_BACK_H
#include <functional>
namespace feipu {
typedef std::function<void()> TimerCallback;
}
#endif