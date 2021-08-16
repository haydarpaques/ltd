cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(ltd_cfg.h.in ltd_cfg.h)

# add the library
add_library( ltd-lib lib/cli_args.cpp lib/errors.cpp lib/log.cpp )
target_include_directories(ltd-lib PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(ltd-lib PUBLIC "${PROJECT_SOURCE_DIR}/inc/")
set_target_properties(ltd-lib PROPERTIES OUTPUT_NAME ltd)

# add the executable
add_executable(ltd-exe app/main.cpp app/utils.cpp)

target_include_directories(ltd-exe PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(ltd-exe PUBLIC "${PROJECT_SOURCE_DIR}/inc/")

target_link_libraries(ltd-exe ltd-lib stdc++fs)

set_target_properties(ltd-exe PROPERTIES OUTPUT_NAME ltd)

# add test executables
add_executable(cliargs-test tests/cliargs.cpp)

target_include_directories(cliargs-test PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(cliargs-test PUBLIC "${PROJECT_SOURCE_DIR}/inc/")

target_link_libraries(cliargs-test ltd-lib)

set_target_properties(cliargs-test PROPERTIES OUTPUT_NAME test_cliargs)

# Test section
enable_testing()

# cliargs test unit
add_test(NAME        cliargs-TC01 COMMAND cliargs-test -c 0)

add_test(NAME        cliargs-TC02 COMMAND cliargs-test)
set_tests_properties(cliargs-TC02 PROPERTIES PASS_REGULAR_EXPRESSION "Usage:.*")

add_test(NAME        cliargs-TC03 COMMAND cliargs-test -c 0)
set_tests_properties(cliargs-TC03 PROPERTIES PASS_REGULAR_EXPRESSION "Case: 0" )

add_test(NAME        cliargs-TC04 COMMAND cliargs-test --testcase 1)
set_tests_properties(cliargs-TC04 PROPERTIES PASS_REGULAR_EXPRESSION "Case: 1" )

add_test(NAME        cliargs-TC05 COMMAND cliargs-test -c 2)
set_tests_properties(cliargs-TC05 PROPERTIES PASS_REGULAR_EXPRESSION "Name: cliargs" )

add_test(NAME        cliargs-TC06 COMMAND cliargs-test -c 3 -v -v)
set_tests_properties(cliargs-TC06 PROPERTIES PASS_REGULAR_EXPRESSION "Verbosity: 2" )

add_test(NAME        cliargs-TC07 COMMAND cliargs-test -c 4 -vvv)
set_tests_properties(cliargs-TC07 PROPERTIES PASS_REGULAR_EXPRESSION "Verbosity: 3" )

