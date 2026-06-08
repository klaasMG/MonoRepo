cmake_minimum_required(VERSION 3.18)
project(AudioEngine)

add_library(AudioEngine src/audio_engine.cpp
        vendor/miniaudio/miniaudio.h
        src/audio_engine.h
)

target_include_directories(AudioEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(AudioEngine PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(AudioEngine PRIVATE common_lib)

link_compiler_library_statically(AudioEngine)