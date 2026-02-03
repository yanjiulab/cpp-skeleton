cmake_minimum_required(VERSION 3.10)
project(cpp14-demo)

add_executable(cpp14_demo main.cpp)
set_target_properties(cpp14_demo PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)
target_include_directories(cpp14_demo PUBLIC ${CMAKE_SOURCE_DIR}/3rd/include)
