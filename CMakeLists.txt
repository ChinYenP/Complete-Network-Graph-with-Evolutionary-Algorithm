cmake_minimum_required(VERSION 3.10)

project(RhythmAI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    AI.cpp
    global.cpp
    binary_addition.cpp
)

add_executable(RhythmAI ${SOURCES})

target_compile_options(RhythmAI PRIVATE -Wall -Wextra)