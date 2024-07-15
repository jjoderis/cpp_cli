cmake_minimum_required(VERSION 3.15)

project(CPP_CLI VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic-errors")

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(EXTERN_DIR "${PROJECT_SOURCE_DIR}/external")
set(BUILD_DIR "${PROJECT_SOURCE_DIR}/build")

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

set(SOURCES
    ${SRC_DIR}/util/CLIException.cpp
    ${SRC_DIR}/CLArgument/CLArgument.cpp
    ${SRC_DIR}/ProgramArguments.cpp
)

add_library(cpp_cli INTERFACE)

target_sources(cpp_cli PUBLIC ${SOURCES})
target_include_directories(cpp_cli INTERFACE src/)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    add_executable(main main.cpp)
    target_link_libraries(main PUBLIC cpp_cli)
endif()