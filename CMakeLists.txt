cmake_minimum_required(VERSION 3.10)

project(minic)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(src)

add_executable(minic
    src/main.cpp
    src/linker/linker.cpp
)


