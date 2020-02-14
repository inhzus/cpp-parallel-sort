cmake_minimum_required(VERSION 3.15)
project(cpp_parallel_sort)

set(CMAKE_CXX_STANDARD 14)
file(GLOB SOURCES src/*.cc)
find_package(Threads REQUIRED)
add_executable(parallel ${SOURCES})
target_link_libraries(parallel Threads::Threads)
