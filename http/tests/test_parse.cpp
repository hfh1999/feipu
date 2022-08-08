#include "HttpHandler.h"
#include <iostream>
#include "Logging.h"
#include "Buffer.h"
#include <cstdio>
#include <unistd.h>
using namespace feipu;
using namespace std;
int main(int argc, char* args[])
{
  if(argc != 2)
    LOG_FATAL << "Must has a arg.";

  const char* filename = args[1];
  cout << filename << endl;

  Logger::setLogLevel(Logger::LoggerLevel::TRACE);
  LOG_INFO << "Start test.";
  Buffer buffer;
  HttpHandler my_handler;
  char tmp_buffer[128];
  char path[256];
  getcwd(path,256);
  cout <<"PWD = "<< path <<endl;
  FILE *file_ptr = fopen(filename, "r");
  if(file_ptr == nullptr)
  {
    LOG_FATAL << "File open error.";
  }
  size_t n;
  // 模拟网络间断读取
  while ((n = fread(tmp_buffer, 1, 128, file_ptr)) != 0) {
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