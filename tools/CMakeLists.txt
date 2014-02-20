
# this is the current version
cmake_minimum_required (VERSION 2.8)

# name fot the tools is 'tools', genius
project (eracha_tools)

# be verbose
#set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -pedantic")

# program names
add_executable(edam edam.c)
add_executable(edc edc.c)
add_executable(san2ascii san2ascii.c)
add_executable(pal2gpl pal2gpl.c)

# EOF
