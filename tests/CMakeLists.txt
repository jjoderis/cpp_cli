cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic-errors")

add_subdirectory("${EXTERN_DIR}/googletest" "${BUILD_DIR}/external/googletest") 

set(TEST_FILES
    CLArgument.test.cpp
    ProgramArguments.test.cpp
) 

# link test files against gtest_main 
add_executable(tests ${TEST_FILES})
target_link_libraries(tests gtest gmock gtest_main cpp_cli)
add_test(NAME example_test COMMAND tests)

target_include_directories(tests PUBLIC
    "${SRC_DIR}"
) 