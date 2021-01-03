cmake_minimum_required(VERSION 3.14)
project(memory_pool)

set(CMAKE_CXX_STANDARD 14)

add_executable(memory_chunk test_memory_chunk.cpp)
add_executable(memory_pool test_memory_pool.cpp)
