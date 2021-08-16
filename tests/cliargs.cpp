#include <ltd.h>
 
using namespace ltd;

auto main(int argc, char **argv) -> int
{
    cli_arguments flags;

    int test_case = 0;
    int verbosity = 0;

    log::println("Case: 1111");
    flags.bind(&test_case, 'c', "testcase", "Specify test case number.");
    flags.bind(&verbosity, 'v', "verbose", "Specify verbosity level.");
    flags.parse(argc, argv);

    if (argc == 1) {
        log::println("Usage: cli_args [options...]");
        flags.print_help(4);
        return 0;
    }

    switch (test_case) {
        case 0:
            log::println("Case: 0");
            break;
        case 1:
            log::println("Case: 1");
            break;
        case 2:
            log::println("Name: cliargs");
            break;
        case 3:
            log::println("Verbosity: %d", verbosity);
            break;
        case 4:
            log::println("Verbosity: %d", verbosity);
            break;
        // TODO: test string arguments with double quotes ""
        default:
            log::println("Invalid test case");            
    }
    return 0;
}