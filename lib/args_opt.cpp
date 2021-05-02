#include <iostream>
#include <cstring>
#include <algorithm>

#include "../inc/args_opt.h"

namespace ltd
{
    args_opt::args_opt() 
        : arg_count(0), 
            arg_values(nullptr), 
            parsing_err(error::no_error) 
    { 

    } 

    void args_opt::init(int argc, char *argv[])
    {
        arg_count   = argc;
        arg_values  = argv;
        parsing_err = error::no_error;
    }

    args_opt::args_opt(int argc, char *argv[]) 
        : arg_count(argc), 
            arg_values(argv), 
            parsing_err(error::no_error) 
    {
        
    }

    size_t args_opt::size() const 
    {
        return arg_count;
    }

    ret<std::string,error> args_opt::at(int index) const
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

    ret<char*,int,error> args_opt::parse(char short_opt, const char *long_opt) const
    {
        for (int i=1; i < arg_count; i++)
        {
            int result;
            
            result = read_short(short_opt, arg_values[i]);
            if (result > 1)
                return {nullptr, result, error::no_error};
            else if (result == 0)
            {
                result = read_long(long_opt, arg_values[i]);
                if (result==0)
                    continue;
            }
            
            if (i + 1 < arg_count)
            {
                std::string value = arg_values[i+1];
                bool isnumber = std::all_of(value.begin(), value.end(), ::isdigit);
                
                if(isnumber) 
                    return {nullptr, std::stol(value), error::no_error};    
                else
                    return {arg_values[i+1], -1, error::no_error};
            }    

            return {nullptr, 1, error::no_error};  
        }

        return {nullptr, 0, error::not_found};
    }

    void args_opt::bind(int& arg, int default_value, char short_opt, const char *long_opt, const char *help)
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

    void args_opt::bind(std::string& arg, std::string default_value, char short_opt, const char *long_opt, const char *help)
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

    void args_opt::print_help(int indent) const
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

    error args_opt::last_error() const
    {
        return parsing_err;
    }    
} // namepsace ltd