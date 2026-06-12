cmake_minimum_required(VERSION 3.10)

project(PCL)

add_executable(PCL MainPCL.cpp
        Tokeniser.h
        tokens.h
        GMakePythonParser.cpp
        GMakePythonParser.h)

target_link_libraries(PCL common_lib)