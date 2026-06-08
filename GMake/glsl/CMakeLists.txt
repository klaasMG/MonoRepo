cmake_minimum_required(VERSION 3.18)
project(GLSL LANGUAGES CXX)

add_library(GLSL glsl.cpp
        glsl.h)

target_link_libraries(GLSL PRIVATE common_lib)

link_compiler_library_statically(GLSL)