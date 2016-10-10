cmake_minimum_required(VERSION 2.6)
project (FireMerge)

include_directories("./src")

set(CMAKE_CXX_FLAGS "-std=c++11 -O3")
add_executable(FireMerge src/merge.cpp)

install(TARGETS FireMerge DESTINATION bin)
