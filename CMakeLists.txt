cmake_minimum_required(VERSION 3.0.0)
project(feipu CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CXX_FLAGS
 #-g
 -O2
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
set(FEIPU_BASE_INCLUDE ${PROJECT_SOURCE_DIR}/base/include)
set(FEIPU_HTTP_INCLUDE ${PROJECT_SOURCE_DIR}/http)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set(SNIPPET_DIR ${PROJECT_SOURCE_DIR}/snippet)
set(HTTP_DIR ${PROJECT_SOURCE_DIR}/http)
set(BASE_DIR ${PROJECT_SOURCE_DIR}/base)

include_directories(${FEIPU_BASE_INCLUDE})
include_directories(${FEIPU_HTTP_INCLUDE})
add_subdirectory(${TEST_DIR})
add_subdirectory(${SNIPPET_DIR})
add_subdirectory(${HTTP_DIR})
add_subdirectory(${BASE_DIR})
