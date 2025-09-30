#######################################
# Project settings
#######################################
cmake_minimum_required(VERSION 3.10)
project(MYPROJECT VERSION 0.1.0)

# C++ standard can be overridden when this is used as a sub-project.
if(NOT CMAKE_CXX_STANDARD)
    # This project requires C++14.
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif(NOT CMAKE_CXX_STANDARD)

# set options
option(BUILD_LIB "build library" OFF)
option(BUILD_SHARED "build shared library" OFF)
option(BUILD_STATIC "build static library" OFF)
option(BUILD_EXAMPLES "build examples" OFF)
option(BUILD_UNITTEST "build unittest" OFF)


# include(CheckIncludeFile)
# check_include_file("unistd.h" HAVE_UNISTD_H)

# include(CheckLibraryExists)
# check_library_exists(libevent crc32c_value "" HAVE_CRC32C)
# check_library_exists(snappy snappy_compress "" HAVE_SNAPPY)
# check_library_exists(zstd zstd_compress "" HAVE_ZSTD)
# check_library_exists(tcmalloc malloc "" HAVE_TCMALLOC)

# set(CMAKE_EXE_LINKER_FLAGS "-static")
# unset(CMAKE_EXE_LINKER_FLAGS)
set(CMAKE_BUILD_TYPE Debug)

# Copy a file to another location and modify its contents.
# Usually used to dynamically generate configuration files.
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h
)

# Construct the build directory structure

set(LIBDIR lib)
set(BINDIR bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BINDIR})
file(INSTALL etc DESTINATION ${CMAKE_BINARY_DIR})
file(INSTALL etc DESTINATION ${CMAKE_BINARY_DIR}/${BINDIR})
file(INSTALL doc/CHANGELOG.md DESTINATION ${CMAKE_BINARY_DIR}/doc)
file(INSTALL doc/USAGE.md DESTINATION ${CMAKE_BINARY_DIR}/doc)

# Set the header file and library search path
set(INCDIRS . include 3rd/include src)
set(LIBDIRS . lib 3rd/lib)
include_directories(${INCDIRS})
link_directories(${LIBDIRS})
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
# Internel dependencies settings
#######################################
find_package(spdlog QUIET)
if(spdlog_FOUND)
    message(STATUS "Found compiled spdlog library ${spdlog_VERSION}")
endif()

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

foreach(module ${MODULES})
    message(STATUS "Detecting ${module} module")
    if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/src/${module})
        set(${module}_DIR ${PROJECT_SOURCE_DIR}/src/${module})
        file(GLOB ${module}_SOURCES
            ${${module}_DIR}/*.c
            ${${module}_DIR}/*.cc
            ${${module}_DIR}/*.cpp
        )
        file(GLOB ${module}_HEADERS
            ${${module}_DIR}/*.h
            ${${module}_DIR}/*.hpp
        )

        if(EXISTS ${PROJECT_SOURCE_DIR}/src/${module}/main.c)
            set(${module}_MAIN ${PROJECT_SOURCE_DIR}/src/${module}/main.c)
            list(REMOVE_ITEM ${module}_SOURCES ${${module}_MAIN})
        endif()

        if(EXISTS ${PROJECT_SOURCE_DIR}/src/${module}/main.cpp)
            set(${module}_MAIN ${PROJECT_SOURCE_DIR}/src/${module}/main.cpp)
            list(REMOVE_ITEM ${module}_SOURCES ${${module}_MAIN})
        endif()

        if(EXISTS ${PROJECT_SOURCE_DIR}/src/${module}/main.cc)
            set(${module}_MAIN ${PROJECT_SOURCE_DIR}/src/${module}/main.cc)
            list(REMOVE_ITEM ${module}_SOURCES ${${module}_MAIN})
        endif()

        # message(STATUS "Setting ${module}_DIR: ${${module}_DIR}")
        # message(STATUS "Setting ${module}_MAIN: ${${module}_MAIN}")
        # message(STATUS "Setting ${module}_SOURCES : ${${module}_SOURCES}")
        # message(STATUS "Setting ${module}_HEADERS : ${${module}_HEADERS}")
    else()
        list(APPEND ROOT_SOURCES ${PROJECT_SOURCE_DIR}/src/${module})
    endif()
endforeach()

#######################################
# cppdemo executable module settings
#######################################
# set(CMAKE_EXE_LINKER_FLAGS "-static")
set(APP_NAME "cppdemo")
add_executable(${APP_NAME}
    ${${APP_NAME}_MAIN}
    ${${APP_NAME}_SOURCES}
)
if(spdlog_FOUND)
    target_link_libraries(${APP_NAME} PRIVATE spdlog::spdlog)
endif()
# target_include_directories(${APP_NAME} PUBLIC ${${APP_NAME}_DIR})
# target_include_directories(${APP_NAME} PRIVATE ${LIBEVENT_INCLUDE_DIRS})
# unset(CMAKE_EXE_LINKER_FLAGS)

#######################################
# started executable module settings
#######################################
set(STARTED_NAME "started")
add_executable(${STARTED_NAME}
    ${${STARTED_NAME}_MAIN}
    ${${STARTED_NAME}_SOURCES}
    ${util_SOURCES}
)

#######################################
# app executable module settings
#######################################
set(APP_NAME "app")
add_executable(${APP_NAME}
    ${${APP_NAME}_MAIN}
    # ${${APP_NAME}_SOURCES}
    # ${util_SOURCES}
)

# target_include_directories(${APP_NAME} PUBLIC
#     ${util_DIR}
# )

# target_include_directories(${APP_NAME} PRIVATE ${LIBEVENT_INCLUDE_DIRS})
# target_link_libraries(${APP_NAME} PRIVATE ${LIBEVENT_LIBRARIES})

#######################################
# net daemon executable module settings
#######################################

set(DAE_NAME "daemon")
add_executable(${DAE_NAME}
    ${${DAE_NAME}_MAIN}
    ${${DAE_NAME}_SOURCES}
)

target_link_libraries(${DAE_NAME} dw)
# target_link_options(${DAE_NAME} PRIVATE -ldw)
# target_compile_options(${DAE_NAME} PRIVATE -g)
# target_link_options(${DAE_NAME} PRIVATE -lbfd)
# target_link_options(${DAE_NAME} PRIVATE -static-libstdc++)

#######################################
# monster executable module settings
#######################################
set(MONSTER_NAME "monster")
add_executable(${MONSTER_NAME}
    ${${MONSTER_NAME}_MAIN}
)

#######################################
# lib module settings
#######################################
if(BUILD_LIB)

    set(LIBYJC_NAME "yjc")
    set(LIBYJC_HEADERS config.h ${util_HEADERS})
    file(INSTALL ${LIBYJC_HEADERS} DESTINATION include/${LIBYJC_NAME})
    file(INSTALL ${LIBYJC_HEADERS} DESTINATION ${PROJECT_SOURCE_DIR}/include/${LIBYJC_NAME})

    if(BUILD_SHARED)
        add_library(yjc SHARED ${util_SOURCES})
        target_compile_definitions(yjc PRIVATE HV_DYNAMICLIB)
        target_include_directories(yjc PRIVATE ${util_DIR}
            INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
        install(TARGETS yjc
            EXPORT libyjcConfig
            ARCHIVE DESTINATION lib
            LIBRARY DESTINATION lib
            RUNTIME DESTINATION bin)
        add_custom_target(libyjc DEPENDS yjc)
    endif()

    if(BUILD_STATIC)
        add_library(yjc_static STATIC ${util_SOURCES})
        target_compile_definitions(yjc_static PUBLIC HV_STATICLIB)
        target_include_directories(yjc_static PRIVATE ${util_DIR}
            INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
        install(TARGETS yjc_static
            EXPORT libyjcConfig
            ARCHIVE DESTINATION lib)
        add_custom_target(libyjc_static DEPENDS yjc_static)
    endif()

    install(FILES ${util_HEADERS} DESTINATION include/yjc)
    install(EXPORT libyjcConfig DESTINATION lib/cmake/libyjc)

endif()

# if(BUILD_SHARED)
#     set(YJC_LIBRARIES yjc CACHE INTERNAL "link yjc libraries")
# else()
#     # add_definitions(-DYJC_STATICLIB)
#     set(YJC_LIBRARIES yjc_static ${LIBS} CACHE INTERNAL "link yjc libraries")
# endif()