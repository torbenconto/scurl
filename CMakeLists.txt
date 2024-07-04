cmake_minimum_required(VERSION 3.28)
project(scurl)

set(CMAKE_CXX_STANDARD 26)

add_executable(scurl main.cpp
        strings.cpp
        strings.hpp)
