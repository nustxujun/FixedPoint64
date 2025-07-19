cmake_minimum_required(VERSION 3.10)

project(fixed64)

set (HEADERS
    "include/fixed64.hpp"
    "include/trig_lut.hpp"
)

set (SOURCES
    "test/common_tests.cpp"
    "test/main.cpp"
    "benchmark/benchmark.cpp"
)

set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS_RELEASE "-O2")
set (CMAKE_CXX_STANDARD 20)
include_directories("include")
add_executable(fixed64 ${SOURCES})
