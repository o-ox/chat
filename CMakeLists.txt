cmake_minimum_required(VERSION 3.0)
project(simple_chat)

set(SOURCES main.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

add_executable(chat ${SOURCES})