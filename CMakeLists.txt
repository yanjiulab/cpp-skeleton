#######################################
# Project settings
#######################################
cmake_minimum_required(VERSION 3.10)
project(CPPSKELETON VERSION 0.1.0)

# C++ standard can be overridden when this is used as a sub-project.
if(NOT CMAKE_CXX_STANDARD)
    # This project requires C++11.
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif(NOT CMAKE_CXX_STANDARD)

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

# add_subdirectory(${CMAKE_SOURCE_DIR}/src/app)
# add_subdirectory(${CMAKE_SOURCE_DIR}/src/lynx)
# add_subdirectory(${CMAKE_SOURCE_DIR}/src/sgmrp)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/t)

# set options
# option(BUILD_LIB "build library" OFF)
# option(BUILD_SHARED "build shared library" OFF)
# option(BUILD_STATIC "build static library" OFF)
# option(BUILD_EXAMPLES "build examples" OFF)
# option(BUILD_UNITTEST "build unittest" OFF)


# include(CheckIncludeFile)
# check_include_file("unistd.h" HAVE_UNISTD_H)

# include(CheckLibraryExists)
# check_library_exists(libevent crc32c_value "" HAVE_CRC32C)
# check_library_exists(snappy snappy_compress "" HAVE_SNAPPY)
# check_library_exists(zstd zstd_compress "" HAVE_ZSTD)
# check_library_exists(tcmalloc malloc "" HAVE_TCMALLOC)

# set(CMAKE_EXE_LINKER_FLAGS "-static")
# unset(CMAKE_EXE_LINKER_FLAGS)
# set(CMAKE_BUILD_TYPE Debug)

# Copy a file to another location and modify its contents.
# Usually used to dynamically generate configuration files.
# configure_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
#     ${CMAKE_CURRENT_SOURCE_DIR}/config.h
# )

# # Construct the build directory structure
# set(LIBDIR lib)
# set(BINDIR bin)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LIBDIR})
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LIBDIR})
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BINDIR})
# file(INSTALL etc DESTINATION ${CMAKE_BINARY_DIR})
# file(INSTALL etc DESTINATION ${CMAKE_BINARY_DIR}/${BINDIR})
# file(INSTALL doc/CHANGELOG.md DESTINATION ${CMAKE_BINARY_DIR}/doc)
# file(INSTALL doc/USAGE.md DESTINATION ${CMAKE_BINARY_DIR}/doc)

# # Set the header file and library search path
# set(INCDIRS . include 3rd/include src)
# set(LIBDIRS . lib 3rd/lib)
# include_directories(${INCDIRS})
# link_directories(${LIBDIRS})
# # message("CMAKE_SOURCE_DIR:         ${CMAKE_SOURCE_DIR}")
# # message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
# # message("PROJECT_SOURCE_DIR:       ${PROJECT_SOURCE_DIR}")
# # message("CMAKE_BINARY_DIR:         ${PROJECT_BINARY_DIR}")
# # message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
# # message("PROJECT_BINARY_DIR:       ${PROJECT_BINARY_DIR}")
# # message("CMAKE_INSTALL_PREFIX:     ${CMAKE_INSTALL_PREFIX}")
# # message("CMAKE_INSTALL_INCLUDEDIR: ${CMAKE_INSTALL_INCLUDEDIR}")
# # message("CMAKE_INSTALL_LIBDIR:     ${CMAKE_INSTALL_LIBDIR}")
# # message("CMAKE_INSTALL_BINDIR:     ${CMAKE_INSTALL_BINDIR}")
