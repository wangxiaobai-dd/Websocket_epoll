cmake_minimum_required(VERSION 3.3)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

project(TCPServer)

include_directories(SYSTEM /usr/include)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++2a -Wall -Wextra -g -O0")

file(GLOB SOURCE_FILES "*.cpp")
list(FILTER SOURCE_FILES EXCLUDE REGEX "TCPClient.cpp")

add_executable(TCPServer ${SOURCE_FILES})
add_executable(TCPClient TCPClient.cpp)

