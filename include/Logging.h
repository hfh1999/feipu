#ifndef LOGGING_H
#define LOGGING_H
#include <functional>
namespace feipu {
/*log前端*/
class Logger {
public:
  Logger() = default;
  ~Logger();
  typedef std::function<void(const char *msg, int len)> OutputCall;
  static void setOutput(OutputCall func) { outputfunc_ = func; }

private:
  static OutputCall outputfunc_;
};
} // namespace feipu
#endif