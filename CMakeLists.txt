cmake_minimum_required(VERSION 3.15)

project(ReadersWriters)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-format")

add_executable(test main.cpp)

set_property(TARGET test PROPERTY CXX_STANDARD 17)

target_link_libraries(test PRIVATE pthread)

