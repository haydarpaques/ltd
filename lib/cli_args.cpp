#include <iostream>
#include <cstring>
#include <algorithm>

#include "../inc/cli_args.h"

namespace ltd
{
    cli_args::cli_args() 
        : arg_count(0), 
            arg_values(nullptr), 
            parsing_err(error::no_error) 
    { 

    } 

    void cli_args::init(int argc, char *argv[])
    {
        arg_count   = argc;
        arg_values  = argv;
        parsing_err = error::no_error;
    }

    cli_args::cli_args(int argc, char *argv[]) 
        : arg_count(argc), 
            arg_values(argv), 
            parsing_err(error::no_error) 
    {
        
    }

    size_t cli_args::size() const 
    {
        return arg_count;
    }

    ret<std::string,error> cli_args::at(int index) const
    {
        if (index >= 0  && index < arg_count)
            return {std::string(arg_values[index]), error::no_error};
        
        return {"", error::index_out_of_bound};
    }

    int read_short(char short_opt, const char* opt)
    {
        int counter = 0;
        if (*opt++ == '-')
        {
            do 
            {
                if (*opt == short_opt)
                    counter++;
            } while (*opt++ != 0);
        }
        return counter;
    }

    int read_long(const char* long_opt, const char* opt)
    {
        if(strlen(opt)<2) return 0;
        opt +=2;
        return !strcmp(long_opt,opt);
    }

    ret<std::vector<std::string>,int,error> cli_args::parse(char short_opt, const char *long_opt) const
    {
        int                      counter_arg = 0;
        std::vector<std::string> values;

        for (int i=1; i < arg_count; i++)
        {
            if (arg_values[i][0] != '-' && counter_arg > 0) {
                std::string str_arg = arg_values[i];
                values.push_back(str_arg);
            } else {
                int result = read_short(short_opt, arg_values[i]);      // reads for short options
                                                                        // check for multiple options i.e. '-vvv' in verbose option
                if (result == 0) {
                    result = read_long(long_opt, arg_values[i]);
                    if (result == 0)
                        continue;
                }

                counter_arg += result;
            }
            
            /*
            if (i + 1 < arg_count)
            {
                std::string value = arg_values[i+1];
                 bool isnumber = std::all_of(value.begin(), value.end(), ::isdigit);
                
                if(isnumber) 
                    return {nullptr, std::stol(value), error::no_error};    
                else
                    return {arg_values[i+1], -1, error::no_error};
            } 
            */   
        }

        return {values, counter_arg, counter_arg > 0 ? error::no_error : error::not_found};  
    }

    void cli_args::bind(int& arg, int default_value, char short_opt, const char *long_opt, const char *help)
    {
        auto [str,num,err] = parse(short_opt, long_opt);
        if (err == error::no_error)
            arg = num;
        else if (err == error::not_found)
            arg = default_value;
        else
            parsing_err = err;

        entries.push_back({short_opt, long_opt, help});
    }

    void cli_args::bind(std::string& arg, std::string default_value, char short_opt, const char *long_opt, const char *help)
    {
        auto [str,num,err] = parse(short_opt, long_opt);
        if (err == error::no_error)
            arg = num;
        else if (err == error::not_found)
            arg = default_value;
        else
            parsing_err = err;

        entries.push_back({short_opt, long_opt, help});
    }

    void cli_args::print_help(int indent) const
    {
        std::string indentation;

        for(int i=0; i<indent; i++)
            indentation += " ";
        
        for(auto [short_opt,long_opt,help] : entries) 
        {
            std::string line;
            line += indentation;
            line += "-";
            line += short_opt;
            line += ", --";
            line += long_opt;
            if (line.length()-indent < 12)
                line += "\t\t";
            else
                line += "\t";
            line += help;
            std::cout << line << std::endl;
        }
    } 

    error cli_args::last_error() const
    {
        return parsing_err;
    }    
} // namepsace ltd