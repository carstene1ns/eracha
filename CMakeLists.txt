###############################################################################
# CMakeLists.txt for eracha
# written by carstene1ns, 2014,2021
###############################################################################

cmake_minimum_required(VERSION 3.13)

# set build type to debug, if not set
if(DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
else()
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
endif()

project(eracha C)

# be verbose
#set(CMAKE_VERBOSE_MAKEFILE on)

# for both c and c++
set(WARNING_FLAGS "-Wall -Wextra -pedantic")
# for c files
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 ${WARNING_FLAGS}")

# pkg-config finds our needed libraries
include(FindPkgConfig)

# needed libraries: sdl2, sdl2_mixer and sdl2_image
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)

# programs
add_executable(eracha
  src/audio.c
  src/video.c
  src/event.c
  src/level.c
  src/file.c
  src/config.c
  src/main.c
)
target_include_directories(eracha PUBLIC
  external
  ${SDL2_MIXER_INCLUDE_DIRS}
  ${SDL2_IMAGE_INCLUDE_DIRS}
  ${SDL2_INCLUDE_DIRS}
)
target_link_libraries(eracha
  minIni
  ${SDL2_MIXER_LIBRARIES}
  ${SDL2_IMAGE_LIBRARIES}
  ${SDL2_LIBRARIES}
)

# libraries
add_library(minIni STATIC
  external/minIni/dev/minIni.c
)
# source code formatter
add_custom_target(source-format
  COMMAND astyle --options=none --suffix=none --style=google --max-code-length=90 --pad-oper
    --break-after-logical --convert-tabs --indent=spaces=2 --indent-switches  --unpad-paren
    --align-pointer=name "${CMAKE_SOURCE_DIR}/src/*.c" "${CMAKE_SOURCE_DIR}/src/*.h"
  COMMENT "Applying source code format..." VERBATIM
)

# tools
option(ENABLE_TOOLS "Enable the tools" OFF)
if(ENABLE_TOOLS)
  add_subdirectory(reverse_engineering/tools)
endif()
