cmake_minimum_required(VERSION 3.13)

project(shell-starter-cpp)

# Set C++ standard
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files for main executable
set(SOURCE_FILES main.cpp src/tokenize.cpp)

# Create main executable
add_executable(shell ${SOURCE_FILES})

# Create tokenizer library
add_library(tokenizer
    src/tokenize.cpp
)

target_include_directories(tokenizer
    PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

# Testing setup
enable_testing()
find_package(Catch2 REQUIRED)

# Create test executable
add_executable(tokenizer_test
    tests/tokenize_test.cpp
)

target_include_directories(tokenizer_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(tokenizer_test
    PRIVATE
    Catch2::Catch2
    tokenizer
)

# Add the test to CTest
add_test(NAME tokenizer_test COMMAND tokenizer_test)

