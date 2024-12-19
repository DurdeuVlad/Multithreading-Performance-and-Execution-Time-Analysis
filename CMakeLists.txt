cmake_minimum_required(VERSION 3.29)
project(proiect_nou)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")

add_executable(proiect_nou main.cpp Algorithm.cpp)

target_include_directories(proiect_nou PRIVATE ${CMAKE_SOURCE_DIR})
