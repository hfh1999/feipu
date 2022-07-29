#include "HttpHandler.h"
#include <iostream>
#include "Logging.h"
#include "Buffer.h"
#include <cstdio>
#include <unistd.h>
using namespace feipu;
using namespace std;
int main()
{
  Logger::setLogLevel(Logger::LoggerLevel::TRACE);
  LOG_INFO << "Start test.";
  Buffer buffer;
  HttpHandler my_handler;
  char tmp_buffer[64];
  char path[256];
  getcwd(path,256);
  cout <<"PWD = "<< path <<endl;
  FILE *file_ptr = fopen("test_req1.txt", "r");
  if(file_ptr == nullptr)
  {
    LOG_FATAL << "File open error.";
  }
  size_t n;
  // 模拟网络间断读取
  while ((n = fread(tmp_buffer, 64, 1, file_ptr)) != 0) {
    buffer.append(tmp_buffer, n);
    HttpHandler::ParseStatus status = my_handler.parse_http(&buffer);
    if (status == HttpHandler::PARSE_ERROR) {
      LOG_INFO << "Parse Error!!!";
    } else if (status == HttpHandler::PARSE_RECV) {
        LOG_INFO << "Need More Data.";
      continue;
    } else if (status == HttpHandler::PARSE_NEXT) {
      LOG_FATAL << "Parse http : INNER ERROR";
    }
    else {
      break;
    }
  }
  return 0;
}