cmake_minimum_required(VERSION 3.15)

# HYPERBUFFER

###########################################################
# MAIN
project(HyperBuffer)

# Set C++ standard to C++14
set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

###########################################################
# TESTS
file(GLOB_RECURSE source "source/*.h*" "source/*.c*")
file(GLOB_RECURSE source_test "test/*.h*" "test/*.c*")
add_executable(HyperBufferTest ${source} ${source_test})

target_include_directories(HyperBufferTest PRIVATE source)
target_include_directories(HyperBufferTest PRIVATE test)
