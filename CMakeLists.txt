cmake_minimum_required(VERSION 2.8.9)
cmake_policy(SET CMP0048 NEW)
project(csv2pgserver VERSION 1.0 LANGUAGES CXX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++17")

set(BisonOutput ${CMAKE_SOURCE_DIR}/common/SqlParser.tab.cpp)
set(FlexOutput ${CMAKE_SOURCE_DIR}/common/SqlParser.lex.cpp)


set(APP_EXEC csv2pgserver)
option (WITH_GFLAGS "Use gflags" OFF)
option(BUILD_TESTING "Enable unit tests" OFF) 

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(FLEX_EXECUTABLE /usr/local/opt/flex/bin/flex)
	set(BiSON_EXECUTABLE /usr/local/opt/bison/bin/bison)
endif()
 
add_subdirectory(glog)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

find_package(BISON REQUIRED)
if(BISON_FOUND)
    add_custom_command(
      OUTPUT ${BisonOutput}
      COMMAND ${BISON_EXECUTABLE}
              --defines=${CMAKE_SOURCE_DIR}/common/SqlParser.tab.h
              --output=${BisonOutput}
              ${CMAKE_SOURCE_DIR}/common/SqlParser.y
      DEPENDS ${CMAKE_SOURCE_DIR}/common/SqlParser.y
      COMMENT "Generating SqlParser.tab.cpp"
    )
endif()

find_package(FLEX 2.6.3 REQUIRED)
if(FLEX_FOUND)
    add_custom_command(
      OUTPUT ${FlexOutput}
      COMMAND ${FLEX_EXECUTABLE}
              --outfile=${FlexOutput}
              ${CMAKE_SOURCE_DIR}/common/SqlParser.l
      DEPENDS ${CMAKE_SOURCE_DIR}/common/SqlParser.l
      COMMENT "Generating SqlParser.lex.cpp"
    )
endif()

file(GLOB SOURCES "*.cpp" )
file(GLOB COMMON_SOURCES "common/*.cpp" )
file(GLOB EXECUTION_SOURCES "execution/*.cpp")

include_directories(PUBLIC . glog glog/src)
 
add_library( ExecutionLib STATIC ${EXECUTION_SOURCES} ${COMMON_SOURCES} ${BisonOutput} ${FlexOutput})

add_executable(${APP_EXEC} ${SOURCES} )
target_link_libraries( ${APP_EXEC} ExecutionLib glog::glog)
