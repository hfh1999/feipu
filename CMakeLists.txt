cmake_minimum_required(VERSION 3.0.0)
project(feipu VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(DUOMU_INCLUDE ${PROJECT_SOURCE_DIR}/include)
include_directories(${DUOMU_INCLUDE})
add_library(feipu Eventloop.cpp Channel.cpp Logging.cpp CurrentThread.cpp FeiSocketops.cpp )
add_executable(test test.cpp)
target_link_libraries(test feipu pthread )
