
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(RopeLib VERSION 1.0.0)
add_library(rope STATIC)

target_sources(rope PRIVATE src/rope.cpp)
