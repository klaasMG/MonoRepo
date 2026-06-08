cmake_minimum_required(VERSION 3.18)
project(Logger)

add_library(Logger Logger.cpp Logger.h)

target_include_directories(Logger PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Logger PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(Logger PRIVATE common_lib)
target_link_libraries(Logger PRIVATE fmt::fmt)

link_compiler_library_statically(Logger)