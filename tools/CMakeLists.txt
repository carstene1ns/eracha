# CMakeLists.txt - carstene1ns 03/2014

# this is the current version
cmake_minimum_required (VERSION 2.8)

# name for the tools is 'tools', genius
project (eracha_tools)

# be verbose
#set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_BUILD_TYPE Debug)
# for c files
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
# for both c and c++
add_definitions(-Wall -Wextra -pedantic -g)

# program names
add_executable(edam src/edam.c)
add_executable(edc src/edc.c)
add_executable(san2ascii src/san2ascii.c)
add_executable(pal2gpl src/pal2gpl.c)
add_executable(til2png src/til2png.c)
add_executable(san2png src/san2png.c)
add_executable(sme2ascii src/sme2ascii.c)
add_executable(levelviewer src/levelviewer.c)

# pkg-config finds our needed libraries
include(FindPkgConfig)

# png converters need libpng
find_package(PNG 1.6 REQUIRED)
target_include_directories(til2png PUBLIC ${PNG_INCLUDE_DIR})
target_include_directories(san2png PUBLIC ${PNG_INCLUDE_DIR})
target_link_libraries(til2png ${PNG_LIBRARY})
target_link_libraries(san2png ${PNG_LIBRARY})

# levelviewer needs sdl2, sdl2_ttf and m
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
find_library(M_LIB m)
target_include_directories(levelviewer PUBLIC ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
target_link_libraries(levelviewer ${SDL2_TTF_LIBRARIES} ${SDL2_LIBRARIES} ${M_LIB})

# EOF
