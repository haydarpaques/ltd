#include <iostream>
#include "ltd_cfg.h"

void print_help() 
{
    std::cout << std::endl << std::endl;

    std::cout << "{Ltd.} is building tools for C/C++ source code. ";
    std::cout << " Version " << LTD_VERSION_MAJOR << "." << LTD_VERSION_MINOR << std::endl;
    std::cout << "Usage: ltd command [arguments]" << std::endl;
    std::cout << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  build       compile and build a project" << std::endl;
    std::cout << "  clean       clean a project from binaries and temporary objects" << std::endl;
    std::cout << "  debug       compile and build a project for debugging" << std::endl;
    std::cout << "  package     copy the whole binary, library and header files into one folder" << std::endl;
    std::cout << "  doc         generate documentation for a project" << std::endl;
    std::cout << "  get         get package for github" << std::endl;
    std::cout << "  info        display information of the current environment" << std::endl;
    std::cout << "  search      search package on github" << std::endl;
    std::cout << "  help        print this help" << std::endl;
    std::cout << "  test        run a test unit" << std::endl;
    std::cout << "              ltd test [project/testunit]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
}

int main(int argc, char *argv[])
{
    print_help(); 
    return 0;
}