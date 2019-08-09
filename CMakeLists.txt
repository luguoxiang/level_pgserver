cmake_minimum_required(VERSION 2.8.9)
project(csv2pgserver)

set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cpp" "common/*.cpp" "execution/*.cpp" "common/*.c")
 
add_executable(csv2pgserver ${SOURCES})
target_compile_features(csv2pgserver PUBLIC cxx_std_14)
target_include_directories(csv2pgserver PUBLIC .)
