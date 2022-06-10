#ifndef CALL_BACK_H
#define CALL_BACK_H
#include <functional>
namespace feipu {
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;
using std::placeholders::_6;
using std::placeholders::_7;
using std::placeholders::_8;
using std::placeholders::_9;
using std::placeholders::_10;
typedef std::function<void()> TimerCallback;
}
#endif