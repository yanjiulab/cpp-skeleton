cmake_minimum_required(VERSION 3.10)
project(cpp17-demo)

add_executable(cpp17_demo main.cpp)
set_target_properties(cpp17_demo PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_include_directories(cpp17_demo PUBLIC ${CMAKE_SOURCE_DIR}/3rd/include)
