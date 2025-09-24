# the name of the target os
set(CMAKE_SYSTEM_NAME Linux)
# target arch
set(CMAKE_SYSTEM_PROCESSOR arm)

# set(ENV{PATH} "/opt/Z7_cross_compiler/bin:$ENV{PATH}")
set(TOOLCHAIN_PATH /opt/Z7_cross_compiler) 
set(TOOLCHAIN_PREFIX arm-linux-gnueabihf)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_PREFIX}-g++)
#set(CMAKE_SYSROOT ${TOOLCHAIN_PATH}/sysroot)

# where is the target environment located
set(CMAKE_FIND_ROOT_PATH 
	${TOOLCHAIN_PATH}/sysroot
)

# adjust the default behavior of the FIND_XXX() commands:
# search the program in the host environment
# find_program
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
# find_library
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER) 
# find_path or find_file
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER) 
# find package
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE NEVER)

# setting options
add_compile_options(-Wno-psabi)
add_link_options(-pthread)