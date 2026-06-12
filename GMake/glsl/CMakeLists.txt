cmake_minimum_required(VERSION 3.18)
project(GLSL LANGUAGES CXX)
project(GLSL_TEST LANGUAGES CXX)

if (TESTING)
    add_executable(GLSL_TEST test.cpp glsl.cpp glsl.h)
    set_target_properties(GLSL_TEST PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(GLSL_TEST PRIVATE common_lib)
    target_link_libraries(GLSL_TEST PRIVATE fmt::fmt)
    link_compiler_library_statically(GLSL_TEST)
endif ()

add_library(GLSL glsl.h glsl.cpp)
target_link_libraries(GLSL PRIVATE common_lib)
target_link_libraries(GLSL PRIVATE fmt::fmt)

link_compiler_library_statically(GLSL)