#include <ltd.h>

using namespace ltd;

auto main(int argc, char **argv) -> int
{
    cli_args flags;
    flags.init(argc, argv);

    int test_case = 0;
    int verbosity = 0;

    flags.bind(test_case, -1, 'c', "testcase", "Specify test case number.");
    flags.bind(verbosity, -1, 'v', "verbose", "Specify verbosity level.");

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
            log::println("Name: cli_args");
            break;
        case 3:
            log::println("Verbosity: %d", verbosity);
            break;
        case 4:
            log::println("Verbosity: %d", verbosity);
            break;
        default:
            log::println("Invalid test case");            
    }
    return 0;
}