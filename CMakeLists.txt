cmake_minimum_required(VERSION 3.10)

# set the project name
project(ltd VERSION 0.2)

# specify the C++ standard and comilation flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -pedantic")

add_library(
    lltd
    lib/cli_args.cpp
    lib/errors.cpp
    lib/log.cpp
    lib/memory.cpp
    lib/smart_ptr.cpp
    lib/ref_counter.cpp
    lib/test_unit.cpp
)

target_include_directories(
    lltd
    PUBLIC
    "${PROJECT_SOURCE_DIR}/inc/"
)

add_executable(
    ltd
    app/main.cpp
    app/project.cpp
    app/utils.cpp
)

target_include_directories(
    ltd
    PUBLIC
    "${PROJECT_SOURCE_DIR}"
)

target_link_libraries(ltd lltd stdc++fs)
