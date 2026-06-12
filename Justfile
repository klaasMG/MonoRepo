#!/usr/bin/env just --justfile

#release:
#  cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
#  cmake --build build

set shell := ["powershell.exe", "-Command"]
run_ui:
    cd {{justfile_directory()}}
    gmake test.gmake
    cd the_ui_tree_build; ../.venv/Scripts/python.exe ui_manger.py