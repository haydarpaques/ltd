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
add_executable(cli_args-test tests/cliargs.cpp)

target_include_directories(cli_args-test PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(cli_args-test PUBLIC "${PROJECT_SOURCE_DIR}/inc/")

target_link_libraries(cli_args-test ltd-lib)

set_target_properties(cli_args-test PROPERTIES OUTPUT_NAME cli_args)

# Test section
enable_testing()

# cli_args test unit
add_test(NAME cli_args-Runs COMMAND cli_args-test -c 0)

add_test(NAME cli_args-Usage COMMAND cli_args-test)
set_tests_properties(cli_args-Usage
  PROPERTIES PASS_REGULAR_EXPRESSION "Usage:.*"
  )

add_test(NAME        cli_args-00 COMMAND cli_args-test -c 0)
set_tests_properties(cli_args-00 PROPERTIES PASS_REGULAR_EXPRESSION "Case: 0" )

add_test(NAME        cli_args-01 COMMAND cli_args-test --testcase 1)
set_tests_properties(cli_args-01 PROPERTIES PASS_REGULAR_EXPRESSION "Case: 1" )

add_test(NAME        cli_args-02 COMMAND cli_args-test -c 2)
set_tests_properties(cli_args-02 PROPERTIES PASS_REGULAR_EXPRESSION "Name: cli_args" )

add_test(NAME        cli_args-03 COMMAND cli_args-test -c 3 -v -v)
set_tests_properties(cli_args-03 PROPERTIES PASS_REGULAR_EXPRESSION "Verbosity: 2" )

add_test(NAME        cli_args-04 COMMAND cli_args-test -c 4 -vvv)
set_tests_properties(cli_args-04 PROPERTIES PASS_REGULAR_EXPRESSION "Verbosity: 3" )

