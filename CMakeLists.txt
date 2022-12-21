cmake_minimum_required(VERSION 3.10)

# set the project name
project(ltd VERSION 0.2)

# specify the C++ standard and compilation flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -pedantic")

# set path variables
set(ROODIR ${PROJECT_SOURCE_DIR})
set(INCDIR ${PROJECT_SOURCE_DIR}/inc)
set(LIBDIR ${PROJECT_SOURCE_DIR}/lib)
set(APPDIR ${PROJECT_SOURCE_DIR}/app)
set(TSTDIR ${PROJECT_SOURCE_DIR}/tests)

# add source files
file(GLOB LIBSOURCES ${LIBDIR}/*.cpp)
file(GLOB SOURCES ${APPDIR}/*.cpp)
file(GLOB TESTSOURCES ${TSTDIR}/*.cpp)

# create liblltd.a static library
add_library(lltd STATIC ${LIBSOURCES})
target_include_directories( lltd PUBLIC ${INCDIR})

# create the executable binary
add_executable(ltd ${SOURCES})
target_include_directories(ltd PUBLIC ${ROODIR})
target_link_libraries(ltd lltd stdc++fs)

# create test executable binaries
foreach(TESTSOURCE ${TESTSOURCES})
    string(REPLACE ".cpp" "" EXECPATH ${TESTSOURCE})
    file(RELATIVE_PATH EXECNAME ${TSTDIR} ${EXECPATH})
    add_executable(${EXECNAME} ${TESTSOURCE})
    target_link_libraries(${EXECNAME} lltd stdc++fs)
endforeach(TESTSOURCE ${TESTSOURCES})
