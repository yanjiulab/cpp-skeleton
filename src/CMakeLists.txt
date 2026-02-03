#######################################
# src/CMakeLists.txt
# Organize modules into C++-standard groups and add each module as a subdirectory.
#######################################

cmake_minimum_required(VERSION 3.10)

# Lists of modules by intended C++ standard
set(CPP11_MODULES demo app cli cppdemo daemon monster started t)
set(CPP14_MODULES demo)
set(CPP17_MODULES demo)
set(CPP20_MODULES demo lynx)

function(add_grouped_modules GROUP_DIR MODULE_LIST)
    foreach(mod ${${MODULE_LIST}})
        set(modpath ${CMAKE_CURRENT_SOURCE_DIR}/${GROUP_DIR}/${mod})
        if(EXISTS ${modpath}/CMakeLists.txt)
            add_subdirectory(${modpath})
        elseif(EXISTS ${modpath})
            add_subdirectory(${modpath})
        else()
            message(WARNING "Module directory not found: ${modpath}")
        endif()
    endforeach()
endfunction()

add_grouped_modules(cpp11 CPP11_MODULES)
add_grouped_modules(cpp14 CPP14_MODULES)
add_grouped_modules(cpp17 CPP17_MODULES)
add_grouped_modules(cpp20 CPP20_MODULES)
