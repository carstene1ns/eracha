
# this is the current version
cmake_minimum_required (VERSION 2.8)

# name for the tools is 'tools', genius
project (eracha_tools)

# be verbose
#set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -pedantic")

# program names
add_executable(edam src/edam.c)
add_executable(edc src/edc.c)
add_executable(san2ascii src/san2ascii.c)
add_executable(pal2gpl src/pal2gpl.c)
add_executable(til2png src/til2png.c)

# png converters need libpng
find_package(PNG 1.6 REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(til2png ${PNG_LIBRARY})

# EOF
