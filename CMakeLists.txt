cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)
#MESSAGE(WARNING "set youre own toolchain if it errors this is my code not production if you have a fix tell me")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS} --target=x86_64-w64-mingw32 --sysroot=C:/Users/klaas/cpp_toolchains/mingw")
set(CMAKE_RC_COMPILER "C:/Users/klaas/cpp_toolchains/LLVM/bin/llvm-rc.exe")
project(Superbuild LANGUAGES CXX)
include(ProcessorCount)
ProcessorCount(N)
set(ENV{CMAKE_BUILD_PARALLEL_LEVEL} ${N})

function(link_compiler_library_statically target)

    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" PARENT_SCOPE)
    endif()
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR MINGW)
        target_link_options(${target} PRIVATE -static-libgcc -static-libstdc++ -lpthread -static)
    endif()
endfunction()

option(TESTING "testing" ON)
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