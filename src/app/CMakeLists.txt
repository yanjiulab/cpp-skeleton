project(app VERSION 0.3.0)

# Copy a file to another location and modify its contents.
# Usually used to dynamically generate configuration files.
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h
)

# Setting output architecture of project
# Example:
# ./build/app
# ├── bin
# │   └── app
# ├── doc
# │   ├── CHANGELOG.md
# │   └── USAGE.md
# └── etc
#     └── app.toml
set(LIBDIR lib)
set(BINDIR bin)
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/${LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/${LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/${BINDIR})
file(GLOB CFG_FILES ${CMAKE_SOURCE_DIR}/etc/*${PROJECT_NAME}*)
file(INSTALL ${CFG_FILES} DESTINATION ${OUTPUT_DIR}/etc)
file(GLOB DOC_FILES ${CMAKE_SOURCE_DIR}/doc/${PROJECT_NAME}/*)
file(INSTALL ${DOC_FILES} DESTINATION ${OUTPUT_DIR}/doc)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME}
    ${SOURCES}
)