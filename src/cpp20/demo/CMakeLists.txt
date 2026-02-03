cmake_minimum_required(VERSION 3.10)
project(cpp20-demo)

add_executable(cpp20_demo main.cpp)
set_target_properties(cpp20_demo PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_include_directories(cpp20_demo PUBLIC ${CMAKE_SOURCE_DIR}/3rd/include)
