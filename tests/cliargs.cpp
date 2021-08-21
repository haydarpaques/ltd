#include <string.h>

#include <ltd.h>

using namespace ltd;

class args_factory
{
    int    arg_count;
    char** arg_list;
public:
    args_factory(int argc, const char** argv)
    {
        arg_count = argc;
        arg_list = new char*[arg_count];
        for (int i=0; i<argc; i++) {
            arg_list[i] = strdup(argv[i]);
        }
    }

    virtual ~args_factory() 
    {
        for (int i=0; i<arg_count; i++) {
            free(arg_list[i]);
        }

        delete [] arg_list;
    }

    int count() const { return arg_count; }
    char ** arguments() { return arg_list; }

};

auto main(int argc, char** argv) -> int
{
    test_unit tu;

    tu.test([&tu] () -> void {
        const char* cli[2] = {"cliargs","-vv"};
        args_factory args(2, cli);

        int    my_argc = args.count(); 
        char **my_argv = args.arguments();

        cli_arguments flags;

        int verbosity = 0;

        flags.bind(&verbosity, 'v', "verbose", "Specify verbosity level.");

        flags.parse(my_argc, my_argv);    
        
        tu.expect(verbosity == 2, "Expect verbosity = 2");
    });

    tu.test([&tu] () -> void {
        const char* cli[3] = {"cliargs","-v", "4"};
        args_factory args(3, cli);

        int    my_argc = args.count(); 
        char **my_argv = args.arguments();

        cli_arguments flags;

        int verbosity = 0;

        flags.bind(&verbosity, 'v', "verbose", "Specify verbosity level.");

        flags.parse(my_argc, my_argv);    
        
        tu.expect(verbosity == 4, "Expect verbosity = 4");
    });

    tu.run(argc, argv);
    return 0;
}