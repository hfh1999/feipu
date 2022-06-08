#include "Logging.h"
#include "CurrentThread.h"
#include "Timestamp.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
namespace feipu {
void defaultoutput(const char *msg, int len) {
  size_t n = ::fwrite(msg, 1, len, stdout); // 默认的输出为标准输出
                                            // chenck n?
}
Logger::OutputCall Logger::outPutFunc_ = defaultoutput;
const char *Logger::logLevelStr_[Logger::LOG_LEVEL_NUM] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};
Logger::LoggerLevel Logger::gLogLevel_ = Logger::DEBUG;
Logger::Logger(const char *file, int line, LoggerLevel level)
    : stream_(), basename_(), line_(line), level_(level) {
  // deal head.
  const char *path_sep_pos = ::strrchr(file, '/');
  basename_ =
      (path_sep_pos == nullptr) ? file : path_sep_pos + 1; // 求文件的basename
  stream_ << TimeStamp::now().toFormattedString() << " "
          << CurrentThread::get_tid_string() << logLevelStr_[level] << " ";
}
Logger::Logger(const char *file, int line, LoggerLevel level, const char *func)
    : stream_(), basename_(), line_(line), level_(level) {
  const char *path_sep_pos = ::strrchr(file, '/');
  basename_ =
      (path_sep_pos == nullptr) ? file : path_sep_pos + 1; // 求文件的basename
  stream_ << TimeStamp::now().toFormattedString() << " "
          << CurrentThread::get_tid_string() << logLevelStr_[level] << " ";
}
Logger::~Logger() {
  stream_ << " - " << basename_ << ":" << line_ << '\n';
  std::string str = stream_.str();
  outPutFunc_(str.data(), str.size());
  if (level_ == LoggerLevel::FATAL) {
    abort();
  }
}
} // namespace feipu