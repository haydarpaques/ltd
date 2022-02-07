cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(ltd_cfg.h.in ltd_cfg.h)

# SET(GCC_COVERAGE_COMPILE_FLAGS "-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused")
SET(GCC_COVERAGE_COMPILE_FLAGS "-pedantic")

# add the library
add_library( ltd-lib lib/cli_args.cpp lib/errors.cpp lib/log.cpp lib/memory.cpp lib/smart_ptr.cpp lib/ref_counter.cpp lib/test_unit.cpp )
target_include_directories(ltd-lib PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(ltd-lib PUBLIC "${PROJECT_SOURCE_DIR}/inc/")
set_target_properties(ltd-lib PROPERTIES OUTPUT_NAME ltd)

# add the executable
add_executable(ltd-exe app/main.cpp app/utils.cpp)

target_include_directories(ltd-exe PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(ltd-exe PUBLIC "${PROJECT_SOURCE_DIR}/inc/")

target_link_libraries(ltd-exe ltd-lib stdc++fs)

set_target_properties(ltd-exe PROPERTIES OUTPUT_NAME ltd)
get_target_property(TEMP ltd-exe COMPILE_FLAGS)
if(TEMP STREQUAL "TEMP-NOTFOUND")
  SET(TEMP "") # Set to empty string
else()
  SET(TEMP "${TEMP} ") # A space to cleanly separate from existing content
endif()
# Append our values
SET(TEMP "${TEMP}${GCC_COVERAGE_COMPILE_FLAGS}" )
set_target_properties(ltd-exe PROPERTIES COMPILE_FLAGS ${TEMP})


# add test executables
add_executable(cliargs-test tests/cliargs.cpp)
target_include_directories(cliargs-test PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(cliargs-test PUBLIC "${PROJECT_SOURCE_DIR}/inc/")
target_link_libraries(cliargs-test ltd-lib)
set_target_properties(cliargs-test PROPERTIES OUTPUT_NAME test_cliargs)

add_executable(pointers-test tests/pointers.cpp)
target_include_directories(pointers-test PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(pointers-test PUBLIC "${PROJECT_SOURCE_DIR}/inc/")
target_link_libraries(pointers-test ltd-lib)
set_target_properties(pointers-test PROPERTIES OUTPUT_NAME test_pointers)

# Test section
enable_testing()

# define a function to simplify adding tests
function(do_test target arg result)
  add_test(NAME ${${target}}${arg} COMMAND ${${target}} ${arg})
  set_tests_properties(${${target}}${arg}
    PROPERTIES PASS_REGULAR_EXPRESSION ${result}
    )
endfunction(do_test)

function(config_test_bin testbin)        
    execute_process(COMMAND "./${testbin}"
        OUTPUT_VARIABLE out1)

    math(EXPR LOOP_STOP "${out1} - 1")

    foreach(X RANGE 0 ${LOOP_STOP})
        do_test(testbin ${X} "-ok-")
    endforeach()
endfunction(config_test_bin)

config_test_bin(test_cliargs)
config_test_bin(test_pointers)