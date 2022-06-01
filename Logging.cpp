#include "Logging.h"
#include <cstdio>
namespace feipu {
    void defaultoutput(const char* msg,int len){
        size_t n = ::fwrite(msg,1,len,stdout);
        // chenck n?
    }
    Logger::OutputCall Logger::outputfunc_ = defaultoutput;

    Logger::~Logger()
    {
        
    }
}