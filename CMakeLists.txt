cmake_minimum_required(VERSION 3.0.0)
project(feipu CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CXX_FLAGS
 -g
 # -DVALGRIND
 # -DMUDUO_STD_STRING
 #-Wall
 -Wextra
 # -m32
 -Werror
 #-Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
 # -std=c++0x
 -rdynamic
 -pthread
 )
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(DUOMU_INCLUDE ${PROJECT_SOURCE_DIR}/include)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set(SNIPPET_DIR ${PROJECT_SOURCE_DIR}/snippet)

set(feipu_SRCS
Eventloop.cpp
Channel.cpp 
Logging.cpp 
CurrentThread.cpp 
FeiSocketops.cpp
InetAddress.cpp
TcpServer.cpp
TcpConnection.cpp
Buffer.cpp
)
include_directories(${DUOMU_INCLUDE})
add_library(feipu ${feipu_SRCS})
add_subdirectory(${TEST_DIR})
add_subdirectory(${SNIPPET_DIR})
