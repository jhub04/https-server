cmake_minimum_required(VERSION 3.30)
project(https_server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -pthread -Wall -Wextra")

if(APPLE)
    include_directories(/usr/local/include /opt/homebrew/include)
endif()

add_executable(https_server main.cpp HttpServer.cpp HttpServer.h)
