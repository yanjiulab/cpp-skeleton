cmake_minimum_required(VERSION 3.10)
project(c-project-template)

file(GLOB COMMON_SOURCES src/common/*.c)

# add header file search path
include_directories(
    include
    src/common
)

#
set(APP_NAME "app")
set(APP_FULL_NAME "${CMAKE_SYSTEM_PROCESSOR}_${APP_NAME}")
add_executable(${APP_FULL_NAME}
    src/app/main.c
    ${COMMON_SOURCES}
)
