###############################################################################
# CMakeLists.txt for eracha tools
# written by carstene1ns, 2014,2021
###############################################################################

cmake_minimum_required (VERSION 3.13)

# set build type to debug, if not set
if(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
else()
   SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
endif()

project (eracha_tools)

# be verbose
#set(CMAKE_VERBOSE_MAKEFILE on)

# for c files
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
# for both c and c++
add_definitions(-Wall -Wextra -pedantic)

# dirty little helper
add_library(helper src/helper.c)

# program names
add_executable(edam src/edam.c)
add_executable(edc src/edc.c)
add_executable(san2ascii src/san2ascii.c)
add_executable(pal2gpl src/pal2gpl.c)
add_executable(pal2png src/pal2png.c)
add_executable(til2png src/til2png.c)
add_executable(san2png src/san2png.c)
add_executable(sme2ascii src/sme2ascii.c)
add_executable(levelviewer src/levelviewer.c)

target_link_libraries(san2ascii helper)
target_link_libraries(pal2gpl helper)
target_link_libraries(sme2ascii helper)

# pkg-config finds our needed libraries
include(FindPkgConfig)

# png converters need libpng
find_package(PNG 1.6 REQUIRED)
target_include_directories(til2png PUBLIC ${PNG_INCLUDE_DIR})
target_include_directories(san2png PUBLIC ${PNG_INCLUDE_DIR})
target_include_directories(pal2png PUBLIC ${PNG_INCLUDE_DIR})
target_link_libraries(til2png helper ${PNG_LIBRARY})
target_link_libraries(san2png helper ${PNG_LIBRARY})
target_link_libraries(pal2png helper ${PNG_LIBRARY})

# levelviewer needs sdl2, sdl2_ttf and m
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_TTF REQUIRED SDL2_ttf)
find_library(M_LIB m)
target_include_directories(levelviewer PUBLIC ${SDL2_TTF_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})
target_link_libraries(levelviewer helper ${SDL2_TTF_LIBRARIES} ${SDL2_LIBRARIES} ${M_LIB})
