cmake_minimum_required(VERSION 3.18)
project(AudioEngine)

set(CMAKE_CXX_STANDARD 23)

add_library(AudioEngine src/main.cpp
        vendor/miniaudio/miniaudio.h)

target_include_directories(AudioEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(AudioEngine PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(AudioEngine PRIVATE common_lib)

if(MINGW)
    target_link_libraries(AudioEngine PRIVATE -static-libgcc -static-libstdc++ -static)
endif()
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()