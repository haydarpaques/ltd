#include <stdio.h>
#include <ltd.h>
#include <regex>

using namespace ltd;

error check_version(const char *name, const char *desc, const char *cmd, const char *rex);
void print_help();
void print_version();