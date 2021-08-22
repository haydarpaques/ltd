#include <stdio.h>
#include <ltd.h>
#include <regex>

using namespace ltd;

/**
 * @brief Checks a version of a tool used with ltd
 * @param name      The name of the tool
 * @param desc      The description of the tool
 * @param cmd       The command line to call the tool
 * @param rex       The regular expression to grab the tool's version
 * @return error    Returns `not_found` if the tools is not found or unable to extract version
 */
error check_version(const char *name, const char *desc, const char *cmd, const char *rex);

/**
 * @brief Prints help to the terminal
 * 
 */
void print_help();

/**
 * @brief Prints ltd version to the terminal
 * 
 */
void print_version();