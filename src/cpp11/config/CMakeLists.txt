get_filename_component(MOD_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(MOD_CXX_STANDARD 11)

file(GLOB_RECURSE MOD_SRCS CONFIGURE_DEPENDS 
     "${CMAKE_CURRENT_SOURCE_DIR}/*.c"          # C source
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"        # C++ source
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"         # Linux C++ source
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx"        # Standard C++ source
     "${CMAKE_CURRENT_SOURCE_DIR}/*.C")         # Uppercase C++ source

if(MOD_SRCS)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
        add_executable(${MOD_NAME} ${MOD_SRCS})
    else()
        add_library(${MOD_NAME} STATIC ${MOD_SRCS})
    endif()
    target_include_directories(${MOD_NAME} PUBLIC ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/3rd/include)
    set_target_properties(${MOD_NAME} PROPERTIES CXX_STANDARD ${MOD_CXX_STANDARD} CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
    target_compile_definitions(${MOD_NAME} PRIVATE YAML_SUPPORT)
endif()