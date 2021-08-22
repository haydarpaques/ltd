#ifndef _LTD_INCLUDE_CLI_ARGS_H_
#define _LTD_INCLUDE_CLI_ARGS_H_

#include <variant>
#include <vector>
#include <string>
#include <map>

#include "errors.h"
#include "stdalias.h"

namespace ltd
{
     /**
     * 
     * @details Class cli_arguments implements command line arguments helper tools.
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
     *              cli_arguments args;
     *              int debug     = 0;
     *              int verbosity = 0;
     *  
     *              args.bind(&debug, 'g', "debug", "Build with debug information.");
     *              args.bind(&verbosity, 'v', "verbose", "Verbose logging.");
     *              args.parse(argc, argv);
     *           
     *              if (arguments.size() < 2)
     *              {
     *                  print_help();
     *                  arguments.print_help(4);
     *                  return 1;
     *              }
     * 
     *              return 0;
     *          }
     * ```
     * 
     * @brief Provides helper functions for handling with command-line arguments.
     */ 
    class cli_arguments
    {
    private:                

        struct option
        {   
            using value_t = std::variant<int*, std::string*, std::vector<const char*>*>;

            std::string long_opt;
            std::string help;
            char        short_opt;
            value_t     value;            
        };

        struct argument
        {
            std::vector<const char*> values;
            size_t occurence;
            char   short_opt;
        };

    private:
        int    argc;
        char** argv;

        std::vector<option>   options;
        std::vector<argument> arguments;

    private:
        ret<int, error> get_argument(char short_opt);
        ret<int, error> add_argument(char short_opt);
        
        error parse_argv();
        error bind_values();

    public:
        /**
         * @brief Construct a new cli args object
         */
        cli_arguments();

        /**
         * Call this function to bind a string variable to an argument. If 
         * the argument is provided in the cli command the variable will be 
         * assigned with that value. If the option is not provided, then a 
         * default value will be assigned to the variable.
         * 
         * @brief Binds a string to an option.
         */ 
        error bind(int* value, char short_opt, const std::string& long_opt, const std::string& help);

        /**
         * Call this function to bind a string variable to an argument. If 
         * the argument is provided in the cli command the variable will be 
         * assigned with that value. If the option is not provided, then a 
         * default value will be assigned to the variable.
         * 
         * @brief Binds a string to an option.
         */ 
        error bind(std::string* value, char short_opt, const std::string& long_opt, const std::string& help);

        /**
         * Call this function to bind a string variable to an argument. If 
         * the argument is provided in the cli command the variable will be 
         * assigned with that value. If the option is not provided, then a 
         * default value will be assigned to the variable.
         * 
         * @brief Binds a string to an option.
         */ 
        error bind(std::vector<const char*>* value, char short_opt, const std::string& long_opt, const std::string& help);

        /**
         * @brief 
         * 
         * @param func 
         */
        void  iterate_options(void (func)(std::variant<int*, std::string*, std::vector<const char*>*>, char, const std::string&, const std::string&));

        /**
         * @brief Get the short opt object
         * 
         * @param long_opt 
         * @return ret<char, error> 
         */
        ret<char, error> get_short_opt(const std::string& long_opt) const;

        /**
         * @brief Get the long opt object
         * 
         * @param short_opt 
         * @return ret<const std::string&, error> 
         */
        ret<const std::string&, error> get_long_opt(char short_opt) const;

        /**
         * @brief Get the index object
         * 
         * @param short_opt 
         * @return ret<int, error> 
         */
        ret<int, error> get_index(char short_opt) const;

        /**
         * @brief 
         * 
         * @param index 
         * @return ret<const char*, error> 
         */
        ret<const char*, error> at(size_t index) const;

        /**
         * @brief 
         * 
         * @param argc 
         * @param argv 
         * @return error 
         */
        error parse(int argc, char** argv);

        /**
         * @brief Get the `argc` value.
         * 
         * @return size_t The value of argc.
         */
        size_t size() const;

        /**
         * @brief Prints options switches to terminal.
         * 
         * Prints all short and long options into the terminal. This can be used
         * with help display.
         * 
         * @param indent Size of the indentation.
         */
        void print_help(size_t indent) const;

    }; // class cli_arguments
} // namespace ltd

#endif // _LTD_INCLUDE_CLI_ARGS_H_