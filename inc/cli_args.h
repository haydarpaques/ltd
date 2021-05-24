#ifndef _LTD_INCLUDE_CLI_ARGS_H_
#define _LTD_INCLUDE_CLI_ARGS_H_

#include <vector>
#include <string>

#include "errors.h"
#include "stdalias.h"

namespace ltd
{
    /**
     * @brief Provides helper functions for handling with command-line arguments.
     *
     * Class cli_args implements command line arguments helper tools.
     * 
     * This class provides command line arguments parser and simple help system. 
     * To use this class, initialize it with `argc` and `argv` taken from `main()`.
     * 
     * Bind local variable with an option by defining short option ('-'), the 
     * corresponding long option ('--') and description for help text.
     * 
     * ```C++
     *          int main(int argc, char *argv[])
     *          {
     *              auto arguments = ltd::cli_args(argc, argv);
     *              int debug     = 0;
     *              int verbosity = 0;
     *  
     *              arguments.bind(debug, 0, 'g', "debug", "Build with debug information.");
     *              arguments.bind(verbosity, 0, 'v', "verbose", "Verbose logging.");
     *           
     *              if (arguments.last_error() != error::no_error || arguments.size() < 2)
     *              {
     *                  print_help();
     *                  arguments.print_help(4);
     *                  return 1;
     *              }
     * 
     *              return 0;
     *          }
     * ```
     */ 
    class cli_args
    {
        int    arg_count    = 0;
        char **arg_values   = nullptr;

        using opt_list = std::vector<ret<char,std::string,std::string>>;  
        opt_list entries;

        error parsing_err;

        ret<std::vector<std::string>,int,error> parse(char short_opt, const char *long_opt) const;

        public:
            
            cli_args();

            void init(int argc, char *argv[]);

            /**
             * @brief
             * Construct the arguments object by providing `argc` and `argv` from `main()`.
             */ 
            cli_args(int argc, char *argv[]);

            /**
             * @brief Returns the number of argument(s) available.
             */ 
            size_t size() const;

            /**
             * @brief Returns the argument at the given index.
             */
            ret<std::string,error> at(int index) const;
            
            /**
             * @brief
             * Binds an integer to an option.
             * 
             * Call this function to bind a variable to a certain option. If the option is provided
             * when the program is launched, the variable will be assigned with that value.
             * If the option is not provided, then a default value will be assigned to the variable.
             */ 
            void bind(int& arg, int default_value, char short_opt, const char *long_opt, const char *help);

            /**
             * @brief
             * Binds a string to an option.
             * 
             * Call this function to bind a variable to a certain option. If the option is provided
             * when the program is launched, the variable will be assigned with that value.
             * If the option is not provided, then a default value will be assigned to the variable.
             */ 
            void bind(std::string& arg, std::string default_value, char short_opt, const char *long_opt, const char *help); 

            /**
             * @brief Print list of options available to the terminal indented.
             */        
            void print_help(int indent) const;  

            /**
             * @brief Check for parsing errors.
             */
            error last_error() const;
    }; // class cli_args
} // namespace ltd

#endif // _LTD_INCLUDE_CLI_ARGS_H_