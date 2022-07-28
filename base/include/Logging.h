#ifndef LOGGING_H
#define LOGGING_H
#include <functional>
#include <sstream>
namespace feipu {
/*log前端*/
//负责log的格式化

//通过相应的后端来达成高级目标，如多线程异步日志
class Logger {
public:
  enum LoggerLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    LOG_LEVEL_NUM,
  };

  typedef std::function<void(const char *msg, int len)> OutputCall;
  typedef std::ostringstream logstream;
  Logger(const char *file, int line, LoggerLevel level);
  Logger(const char *file, int line, LoggerLevel level,const char* func);
  Logger(const char *file, int line, bool toAbort);
  ~Logger();
  static void setOutput(OutputCall func) { outPutFunc_ = func; }
  logstream& stream(){return stream_;}
  static LoggerLevel logLevel(){
    return gLogLevel_;
  }
  static void setLogLevel(LoggerLevel level){
    gLogLevel_ = level;
  }

private:
  static OutputCall outPutFunc_;
  static const char* logLevelStr_[];
  static LoggerLevel gLogLevel_;
  logstream stream_;
  std::string basename_;
  int line_;
  LoggerLevel level_;
};

#define LOG_TRACE if(Logger::logLevel() <= Logger::TRACE) Logger(__FILE__,__LINE__,Logger::TRACE,__func__).stream()
#define LOG_DEBUG Logger(__FILE__,__LINE__,Logger::DEBUG,__func__).stream()
#define LOG_INFO Logger(__FILE__,__LINE__,Logger::INFO).stream()
#define LOG_WARN Logger(__FILE__,__LINE__,Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__,__LINE__,Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__,__LINE__,Logger::FATAL).stream()
#define LOG_SYSFATAL Logger(__FILE__,__LINE__,true).stream()
#define LOG_SYSERROR Logger(__FILE__,__LINE__,false).stream()
} // namespace feipu
#endif