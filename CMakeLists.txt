cmake_minimum_required(VERSION 3.5)
project(nanoswf)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES test.c)
add_executable(nanoswftest ${SOURCE_FILES})