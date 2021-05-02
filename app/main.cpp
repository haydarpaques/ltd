#include <ltd.h>
#include <ltd_cfg.h>

using namespace ltd;

void print_version() 
{
    fmt::println("ltd version %d.%d", LTD_VERSION_MAJOR, LTD_VERSION_MINOR);
}

int main(int argc, char *argv[])
{
    print_version(); 
    return 0;
}