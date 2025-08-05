cmake_minimum_required(VERSION 3.15...4.0)
project(glad)
set(CMAKE_CXX_STANDARD 20)

add_library(glad src/glad.c include/glad/glad.h include/KHR/khrplatform.h )
target_include_directories(glad PRIVATE include)