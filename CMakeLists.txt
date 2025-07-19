#######################################
# Project
#######################################
cmake_minimum_required(VERSION 3.10)
project(proj VERSION 0.1.0)

configure_file(config.h.in config.h)
# message("CMAKE_SOURCE_DIR:         ${CMAKE_SOURCE_DIR}")
# message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
# message("PROJECT_SOURCE_DIR:       ${PROJECT_SOURCE_DIR}")
# message("CMAKE_BINARY_DIR:         ${PROJECT_BINARY_DIR}")
# message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
# message("PROJECT_BINARY_DIR:       ${PROJECT_BINARY_DIR}")
# message("CMAKE_INSTALL_PREFIX:     ${CMAKE_INSTALL_PREFIX}")
# message("CMAKE_INSTALL_INCLUDEDIR: ${CMAKE_INSTALL_INCLUDEDIR}")
# message("CMAKE_INSTALL_LIBDIR:     ${CMAKE_INSTALL_LIBDIR}")
# message("CMAKE_INSTALL_BINDIR:     ${CMAKE_INSTALL_BINDIR}")

#######################################
# C++ header-only libraries
#######################################
include_directories(
    "./include"
)

#######################################
# External dependencies settings
#######################################
# Manual
set(LIBEVENT_INCLUDE_DIRS /usr/local/include)
set(LIBEVENT_LIBRARIES /usr/local/lib/libevent.so)

# Find_package 
#find_package(Libevent REQUIRED)

# target_include_directories(${APP_NAME} PRIVATE ${LIBEVENT_INCLUDE_DIRS})
# target_link_libraries(${APP_NAME} PRIVATE ${LIBEVENT_LIBRARIES})

#######################################
# Module settings
#######################################

set(ROOT_DIR ${PROJECT_SOURCE_DIR}/src)
file(GLOB MODULES RELATIVE ${ROOT_DIR} ${ROOT_DIR}/*)

foreach(subdir ${MODULES})
    if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/src/${subdir})
        set(${subdir}_DIR ${PROJECT_SOURCE_DIR}/src/${subdir})
        file(GLOB ${subdir}_SOURCES 
            ${${subdir}_DIR}/*.c
            ${${subdir}_DIR}/*.cc
            ${${subdir}_DIR}/*.cpp
        )
        file(GLOB ${subdir}_HEADERS
            ${${subdir}_DIR}/*.h
            ${${subdir}_DIR}/*.hpp
        )
        
        if(EXISTS ${PROJECT_SOURCE_DIR}/src/${subdir}/main.c)
            set(${subdir}_MAIN ${PROJECT_SOURCE_DIR}/src/${subdir}/main.c)
            list(REMOVE_ITEM ${subdir}_SOURCES ${${subdir}_MAIN})
        endif()

        if(EXISTS ${PROJECT_SOURCE_DIR}/src/${subdir}/main.cpp)
            set(${subdir}_MAIN ${PROJECT_SOURCE_DIR}/src/${subdir}/main.cpp)
            list(REMOVE_ITEM ${subdir}_SOURCES ${${subdir}_MAIN})
        endif()

        if(EXISTS ${PROJECT_SOURCE_DIR}/src/${subdir}/main.cc)
            set(${subdir}_MAIN ${PROJECT_SOURCE_DIR}/src/${subdir}/main.cc)
            list(REMOVE_ITEM ${subdir}_SOURCES ${${subdir}_MAIN})
        endif()

        message("${subdir}_DIR: ${${subdir}_DIR}")
        message("${subdir}_MAIN: ${${subdir}_MAIN}")
        # message("${subdir}_SOURCES : ${${subdir}_SOURCES}")
        # message("${subdir}_HEADERS : ${${subdir}_HEADERS}")
    else()
        list(APPEND ROOT_SOURCES ${PROJECT_SOURCE_DIR}/src/${subdir})
    endif()
endforeach()
message("ROOT_SOURCES: ${ROOT_SOURCES}")

#######################################
# cppdemo executable module settings
#######################################
set(APP_NAME "cppdemo")
add_executable(${APP_NAME}
    ${${APP_NAME}_MAIN}
    ${${APP_NAME}_SOURCES}
)

target_include_directories(${APP_NAME} PUBLIC
    ${PROJECT_BINARY_DIR}
    ${${APP_NAME}_DIR}
)

#######################################
# app executable module settings
#######################################
set(APP_NAME "app")
add_executable(${APP_NAME}
    ${${APP_NAME}_MAIN}
    ${${APP_NAME}_SOURCES}
    ${util_SOURCES}
)

target_include_directories(${APP_NAME} PUBLIC
    ${PROJECT_BINARY_DIR}
    ${util_DIR}
)


