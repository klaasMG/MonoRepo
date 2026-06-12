cmake_minimum_required(VERSION 3.18)
project(GMake LANGUAGES CXX)
add_subdirectory(glsl)

# Header-only utility library
add_library(utils INTERFACE)

add_executable(GMake build_dependecy_graph.cpp
        SimpleASTGMAKE.h
        SimpleASTGMAKE.cpp
        TokeniserGMAKE.h
        TokeniserGMAKE.cpp
        Tokens.h
        file_utils.h
        file_utils.cpp
        GMakeTypes.h
        GMakeTypes.cpp
        GmakeFunctionParser.cpp
        GmakeFunctionParser.h
        LiteralTypes.h
        ExceptionHandler.h
        GmakeImplementationPhase.h
        GmakeConfigPhase.h)

target_link_libraries(GMake PRIVATE common_lib GLSL)
target_link_libraries(GMake PRIVATE fmt::fmt)

set_target_properties(GMake PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_SOURCE_DIR}/tooling/gmake)

link_compiler_library_statically(GMake)