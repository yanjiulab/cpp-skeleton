cmake_minimum_required(VERSION 3.10)
project(cpp11-demo)

add_executable(cpp11_demo main.cpp)
set_target_properties(cpp11_demo PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)
target_include_directories(cpp11_demo PUBLIC ${CMAKE_SOURCE_DIR}/3rd/include)
