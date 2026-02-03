#######################################
# src/CMakeLists.txt
# Organize modules into C++-standard groups and add each module as a subdirectory.
#######################################

cmake_minimum_required(VERSION 3.10)

# Option to select flat layout (all outputs directly under build/<group>/<module>)
option(FLAT_MODE "Use flat build layout for module outputs (no bin/doc/etc subfolders)" OFF)

# Auto-discover modules under each C++ standard group directory.
set(STD_GROUPS cpp11 cpp14 cpp17 cpp20)

function(add_grouped_modules GROUP_DIR)
    set(GROUP_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/${GROUP_DIR})
    if(NOT IS_DIRECTORY ${GROUP_ROOT})
        return()
    endif()
    file(GLOB CHILDREN RELATIVE ${GROUP_ROOT} CONFIGURE_DEPENDS ${GROUP_ROOT}/*)
    foreach(child ${CHILDREN})
        if(IS_DIRECTORY ${GROUP_ROOT}/${child})
            # expose collection and module name to the subdirectory CMakeLists
            set(MODULE_COLLECTION ${GROUP_DIR} CACHE INTERNAL "Module collection name")
            set(MODULE_NAME ${child} CACHE INTERNAL "Module name")

            # Compute and export module-specific output directories so subprojects inherit layout
            set(_module_base ${CMAKE_BINARY_DIR}/${GROUP_DIR}/${child})
            set(_libdir lib)
            set(_bindir bin)
            if(FLAT_MODE)
                set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${_module_base} CACHE INTERNAL "module lib out")
                set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${_module_base} CACHE INTERNAL "module archive out")
                set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${_module_base} CACHE INTERNAL "module runtime out")
            else()
                set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${_module_base}/${_libdir} CACHE INTERNAL "module lib out")
                set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${_module_base}/${_libdir} CACHE INTERNAL "module archive out")
                set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${_module_base}/${_bindir} CACHE INTERNAL "module runtime out")
            endif()

            add_subdirectory(${GROUP_ROOT}/${child})

            # clear module-specific cache vars to avoid leaking between modules
            unset(MODULE_COLLECTION CACHE)
            unset(MODULE_NAME CACHE)
            unset(CMAKE_LIBRARY_OUTPUT_DIRECTORY CACHE)
            unset(CMAKE_ARCHIVE_OUTPUT_DIRECTORY CACHE)
            unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY CACHE)
        endif()
    endforeach()
endfunction()

foreach(g ${STD_GROUPS})
    add_grouped_modules(${g})
endforeach()
