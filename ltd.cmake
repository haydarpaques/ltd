cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(ltd_cfg.h.in ltd_cfg.h)

# add the library
add_library( ltd-lib lib/cli_args.cpp lib/errors.cpp lib/log.cpp lib/test_unit.cpp )
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

# define a function to simplify adding tests
function(do_test target arg result)
  add_test(NAME Comp${arg} COMMAND ${target} ${arg})
  set_tests_properties(Comp${arg}
    PROPERTIES PASS_REGULAR_EXPRESSION ${result}
    )
endfunction(do_test)

function(config_test_bin testbin)        
    execute_process(COMMAND "./${testbin}"
        OUTPUT_VARIABLE out1)

    math(EXPR LOOP_STOP "${out1} - 1")

    foreach(X RANGE 0 ${LOOP_STOP})
        do_test(cliargs-test ${X} "-ok-")
    endforeach()
endfunction(config_test_bin)

config_test_bin(test_cliargs)