cmake_minimum_required(VERSION 3.18)
project(Logger)

set(CMAKE_CXX_STANDARD 23)

add_library(Logger Logger.cpp Logger.h)

target_include_directories(Logger PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Logger PRIVATE common_lib)

if(MINGW)
    target_link_libraries(Logger PRIVATE -static-libgcc -static-libstdc++ -static)
endif()
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()