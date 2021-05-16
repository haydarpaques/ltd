cmake_minimum_required(VERSION 3.10)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(ltd_cfg.h.in ltd_cfg.h)

# add the library
add_library( ltd-lib lib/args_opt.cpp lib/errors.cpp lib/fmt.cpp )
target_include_directories(ltd-lib PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(ltd-lib PUBLIC "${PROJECT_SOURCE_DIR}/inc/")
set_target_properties(ltd-lib PROPERTIES OUTPUT_NAME ltd)

# add the executable
add_executable(ltd-exe app/main.cpp)

target_include_directories(ltd-exe PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(ltd-exe PUBLIC "${PROJECT_SOURCE_DIR}/inc/")

target_link_libraries(ltd-exe ltd-lib stdc++fs)

set_target_properties(ltd-exe PROPERTIES OUTPUT_NAME ltd)