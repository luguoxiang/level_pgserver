cmake_minimum_required(VERSION 2.8.9)
cmake_policy(SET CMP0048 NEW)
project(level_pgserver VERSION 1.0 LANGUAGES CXX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)

set (CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/lib)

set(BisonOutput ${CMAKE_SOURCE_DIR}/common/SqlParser.tab.cpp)
set(FlexOutput ${CMAKE_SOURCE_DIR}/common/SqlParser.lex.cpp)

set(APP_EXEC level_pgserver)
option (WITH_GFLAGS "Use gflags" OFF)
option(BUILD_TESTING "glog unit tests" OFF) 
option(LEVELDB_BUILD_TESTS "Build LevelDB's unit tests" OFF)
option(LEVELDB_BUILD_BENCHMARKS "Build LevelDB's benchmarks" OFF)
option(LEVELDB_INSTALL "Install LevelDB's header and library" OFF)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(FLEX_EXECUTABLE /usr/local/opt/flex/bin/flex)
	set(BiSON_EXECUTABLE /usr/local/opt/bison/bin/bison)
endif()
 
add_subdirectory(glog)
add_subdirectory(leveldb)
add_subdirectory(abseil-cpp)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

find_package(BISON)
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
else()
    set(BisonOutput "common/SqlParser.tab.cpp")
endif()

find_package(FLEX 2.6.3)
if(FLEX_FOUND)
    add_custom_command(
      OUTPUT ${FlexOutput}
      COMMAND ${FLEX_EXECUTABLE}
              --outfile=${FlexOutput}
              ${CMAKE_SOURCE_DIR}/common/SqlParser.l
      DEPENDS ${CMAKE_SOURCE_DIR}/common/SqlParser.l
      COMMENT "Generating SqlParser.lex.cpp"
    )
else()
    set(FlexOutput "common/SqlParser.lex.cpp")
endif()

file(GLOB SOURCES "*.cpp" )
file(GLOB COMMON_SOURCES "common/*.cpp" )
file(GLOB EXECUTION_SOURCES "execution/*.cpp")
file(GLOB SERVER_SOURCES "server/*.cpp")
file(GLOB PLANBUILDER_SOURCES "planbuilder/*.cpp")

add_library( Common STATIC ${COMMON_SOURCES}  ${BisonOutput} ${FlexOutput})
target_include_directories(Common PUBLIC . glog glog/src abseil-cpp)

add_library( Execution STATIC ${EXECUTION_SOURCES})
target_include_directories(Execution PUBLIC . glog glog/src abseil-cpp leveldb/include)

add_library( PlanBuilder STATIC ${PLANBUILDER_SOURCES})
target_include_directories(PlanBuilder PUBLIC . glog glog/src abseil-cpp leveldb/include)

add_library( Server STATIC ${SERVER_SOURCES})
target_include_directories(Server PUBLIC . glog glog/src abseil-cpp asio-1.12.2/include)
set_target_properties(Server PROPERTIES COMPILE_FLAGS " -DASIO_STANDALONE")

add_executable(${APP_EXEC} ${SOURCES})
set_target_properties(${APP_EXEC} PROPERTIES COMPILE_FLAGS "-DASIO_STANDALONE")

target_link_libraries( ${APP_EXEC} Server PlanBuilder Execution Common glog::glog leveldb absl::strings)
