cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

project(fixed64)

set(HEADERS
    "include/fixed64.hpp"
    "include/trig_lut.hpp"
)

set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_C_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_STANDARD 20)
include_directories("include")

# Test target
add_executable(fixed64_test
    "test/main.cpp"
    "test/common_tests.cpp"
)

enable_testing()
add_test(NAME fixed64_test COMMAND fixed64_test)

# Benchmark target
add_executable(fixed64_benchmark
    "benchmark/benchmark.cpp"
)
