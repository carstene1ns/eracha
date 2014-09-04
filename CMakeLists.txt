
###############################################################################
# CMakeLists.txt
# written by carstene1ns, 2014
###############################################################################

cmake_minimum_required (VERSION 2.8)

# set build type to debug, if not set
if(DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
else(DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
endif(DEFINED CMAKE_BUILD_TYPE)

project(eracha C)

# be verbose
#set(CMAKE_VERBOSE_MAKEFILE on)

# for c files
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
# for both c and c++
add_definitions(-Wall -Wextra -pedantic)

# program names
add_executable(eracha
  src/audio.c
  src/video.c
  src/main.c
)

# pkg-config finds our needed libraries
include(FindPkgConfig)

# needed libraries: sdl2 sdl2_mixer sdl2_image
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)

include_directories(
  ${SDL2_MIXER_INCLUDE_DIRS}
  ${SDL2_IMAGE_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIRS}
)
target_link_libraries(eracha
  ${SDL2_MIXER_LIBRARIES}
  ${SDL2_IMAGE_LIBRARIES}
  ${SDL2_LIBRARIES}
)

# EOF
