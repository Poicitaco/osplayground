cmake_minimum_required(VERSION 3.10)
project(OSPlayground)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")

include_directories(
    ${PROJECT_SOURCE_DIR}/engine
    ${PROJECT_SOURCE_DIR}/sim
    ${PROJECT_SOURCE_DIR}/ui
    ${PROJECT_SOURCE_DIR}/utils
)

file(GLOB_RECURSE SOURCES
    "engine/*.cpp"
    "sim/**/*.cpp"
    "ui/**/*.cpp"
    "utils/*.cpp"
    "main.cpp"
)

add_executable(osplayground ${SOURCES})
