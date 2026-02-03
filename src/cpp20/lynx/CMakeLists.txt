project(lynx VERSION 0.1.0)

# add_definitions(
#     -D_GLIBCXX_USE_CXX11_ABI=0
#     -D__GLIBC_PREREQ=2,27
#     -U_GLIBCXX_USE_CXX20_ABI
#     -D_LIBCPP_DISABLE_AVAILABILITY
# )

# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--version-script=${CMAKE_SOURCE_DIR}/glibc_ver.lds")

# Copy a file to another location and modify its contents.
# Usually used to dynamically generate configuration files.
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h
)

# Mode
if (NOT CMAKE_BUILD_TYPE)
    # set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    # set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Current build type: ${CMAKE_BUILD_TYPE}")

# Setting output architecture of project
set(LIBDIR lib)
set(BINDIR bin)

# Compute module build directory. Prefer MODULE_COLLECTION/MODULE_NAME if provided by src/CMakeLists.
set(MODULE_BUILD_DIR ${CMAKE_BINARY_DIR})
if(DEFINED MODULE_COLLECTION)
    set(MODULE_BUILD_DIR ${MODULE_BUILD_DIR}/${MODULE_COLLECTION})
endif()
if(DEFINED MODULE_NAME)
    set(MODULE_BUILD_DIR ${MODULE_BUILD_DIR}/${MODULE_NAME})
elseif(DEFINED PROJECT_NAME)
    set(MODULE_BUILD_DIR ${MODULE_BUILD_DIR}/${PROJECT_NAME})
endif()

set(OUTPUT_DIR ${MODULE_BUILD_DIR})
if(FLAT_MODE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/${LIBDIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/${LIBDIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/${BINDIR})
endif()

file(GLOB CFG_FILES ${CMAKE_SOURCE_DIR}/etc/*${PROJECT_NAME}*)
file(INSTALL ${CFG_FILES} DESTINATION ${OUTPUT_DIR}/etc)
file(GLOB DOC_FILES ${CMAKE_SOURCE_DIR}/doc/${PROJECT_NAME}/*)
file(INSTALL ${DOC_FILES} DESTINATION ${OUTPUT_DIR}/doc)

# Gathering source files recursively
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Setting executable file
add_executable(${PROJECT_NAME}
    ${SOURCES}
)

# Compiling
# target_compile_options(${PROJECT_NAME} PRIVATE -g)

# Linking
# target_link_libraries(${PROJECT_NAME} dw)
# target_link_options(${PROJECT_NAME} PRIVATE -Wl,--as-needed)
# target_link_options(${PROJECT_NAME} PRIVATE -Wl,-Map,lynx.map)
target_link_options(${PROJECT_NAME} PRIVATE -static-libstdc++ -static-libgcc)
# set(CMAKE_EXE_LINKER_FLAGS "-static") # Full static linking which is not recommended

# Installing
install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin/)
# # Configure .service file
# configure_file(${CMAKE_SOURCE_DIR}/tools/systemd/lynx.service.in ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.service)
# # Install the config file .conf
# install(FILES ${CMAKE_SOURCE_DIR}/etc/${PROJECT_NAME}.toml DESTINATION /etc/)
# # Install the systemd file .service
# install(FILES ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.service DESTINATION /etc/systemd/system/)