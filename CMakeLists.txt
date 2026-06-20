cmake_minimum_required(VERSION 3.24)
project(MiniRTMP)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
        ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(MiniRTMP ${SOURCES})