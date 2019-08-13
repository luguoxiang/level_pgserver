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

FIND_PACKAGE(BISON REQUIRED)
SET(BisonOutput ${CMAKE_SOURCE_DIR}/common/SqlParser.tab.cpp)
IF(BISON_FOUND)
    ADD_CUSTOM_COMMAND(
      OUTPUT ${BisonOutput}
      COMMAND ${BISON_EXECUTABLE}
              --defines=${CMAKE_SOURCE_DIR}/common/SqlParser.tab.h
              --output=${BisonOutput}
              ${CMAKE_SOURCE_DIR}/common/SqlParser.y
      COMMENT "Generating SqlParser.tab.cpp"
    )
ENDIF()

FIND_PACKAGE(FLEX REQUIRED)
SET(FlexOutput ${CMAKE_SOURCE_DIR}/common/SqlParser.lex.cpp)
IF(FLEX_FOUND)
    ADD_CUSTOM_COMMAND(
      OUTPUT ${FlexOutput}
      COMMAND ${FLEX_EXECUTABLE}
              --outfile=${FlexOutput}
              ${CMAKE_SOURCE_DIR}/common/SqlParser.l
      COMMENT "Generating SqlParser.lex.cpp"
    )
ENDIF()

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "*.cpp" "common/*.cpp" "execution/*.cpp")
 
add_executable(csv2pgserver ${SOURCES} ${BisonOutput} ${FlexOutput})
target_compile_features(csv2pgserver PUBLIC cxx_std_14)
target_include_directories(csv2pgserver PUBLIC .)
