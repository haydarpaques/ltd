cmake_minimum_required(VERSION 3.10)

# set the project name
project(ltd VERSION 0.1)

configure_file(ltd_cfg.h.in ltd_cfg.h)

# add the executable
add_executable(ltd apps/ltd/main.cpp)