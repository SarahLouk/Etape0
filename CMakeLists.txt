cmake_minimum_required(VERSION 3.3)
project(Etape0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/terrain.cpp)
add_executable(Etape0 ${SOURCE_FILES})