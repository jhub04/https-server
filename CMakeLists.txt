cmake_minimum_required(VERSION 3.30)
project(https_server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -pthread -Wall -Wextra")

if(APPLE)
    include_directories(/usr/local/include /opt/homebrew/include)
endif()

find_package(OpenSSL)
add_executable(main main.cpp HttpServer.cpp HttpServer.h)
target_link_libraries(main OpenSSL::SSL OpenSSL::Crypto)
