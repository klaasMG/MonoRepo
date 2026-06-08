cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)
project(Superbuild LANGUAGES CXX)
include(ProcessorCount)
ProcessorCount(N)
set(ENV{CMAKE_BUILD_PARALLEL_LEVEL} ${N})

function(link_compiler_library_statically target)
    if(MINGW)
    target_link_libraries(${target} PRIVATE -static-libgcc -static-libstdc++ -static)
endif()
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
endfunction()

option(BUILD_CODE "Build libA" ON)
option(BUILD_PCL "Build libB" ON)
option(BUILD_GMAKE "Build libC" ON)
option(BUILD_ASSET_MANAGER "Build libD" ON)
option(BUILD_AUDIO_ENGINE "Build libE" ON)
option(BUILD_LOGGER "Build libF" ON)
option(BUILD_STRING_FORMATTER "Build libG" ON)

message(STATUS "this is a hotfix and should be changed later")
include_directories(${CMAKE_SOURCE_DIR}/lib/cpp)
message(STATUS "hotfix en is here for the lib libraries")

add_library(common_lib
        lib/cpp_base/GMAKE_EXCEPTION.cpp
        lib/cpp_base/string_utils.h
        lib/cpp_base/string_utils.cpp
        lib/cpp_base/Error.tpp
        lib/cpp_base/Error.h
)

target_include_directories(common_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/lib/cpp_base
)

if (BUILD_STRING_FORMATTER)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/vendor/cpp/fmt)
endif ()

if (BUILD_LOGGER)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/logger)
endif ()

if(BUILD_ASSET_MANAGER)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/asset_manager)
endif()

if(BUILD_GMAKE)
    add_subdirectory(GMake)
endif()

if(BUILD_CODE)
    add_subdirectory(the_code)
endif()

if(BUILD_PCL)
    option(TEST_COMPILATION "test compilation" ON)
    add_subdirectory(PythonCompilerLibrary)
endif()

if (BUILD_AUDIO_ENGINE)
    add_subdirectory(audio_engine)
endif()