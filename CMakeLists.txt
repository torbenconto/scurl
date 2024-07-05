cmake_minimum_required(VERSION 3.28)
project(scurl)

set(CMAKE_CXX_STANDARD 26)

find_package(OpenSSL REQUIRED)

add_executable(scurl main.cpp
        strings.cpp
        strings.hpp
        http.cpp
        http.hpp)

target_link_libraries(scurl OpenSSL::SSL)
