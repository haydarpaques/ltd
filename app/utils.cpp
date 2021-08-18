#include "utils.h"

#include <ltd_cfg.h>

error exec_and_read(const char *command, std::string &out_buffer)
{
    FILE*  fp;
    const int sizebuf=1024;
    char buff[sizebuf];

    if ((fp = popen (command, "r"))== NULL)
        return error::invalid_operation;

    while (fgets(buff, sizeof (buff), fp)) 
        out_buffer += buff;
    
    pclose(fp);

    return error::no_error;
}

error check_version(const char *name, const char *desc, const char *cmd, const char *rex)
{
    std::string out;
    log::println("-- Detecting %s - %s", name, desc);
    auto exec_ret = exec_and_read(cmd, out);
    if (exec_ret == error::no_error) {
        std::regex e(rex);
        std::cmatch cm;

        if (std::regex_search(out.c_str(), cm, e, std::regex_constants::match_any)) {
            log::println("--     %s detected. %s version is %s", name, name, cm[0]);
            return error::no_error;
        }
    } 

    log::println("%s is not detected. Exiting...", name);
    return error::not_found;
}

void print_help()
{
    log::println("Usage: ltd <command> [options...]");
    log::println("");
    log::println("Available commands:");
    log::println("");
    log::println("    build         build a project");
    log::println("    run           run an executable in a project");
    log::println("    clean         clean a project");
    log::println("    version       displays ltd version");
    log::println("    help          displays ltd help");
    log::println("");
    log::println("Options:");
}

void print_version() 
{
    log::println("ltd version %d.%d", LTD_VERSION_MAJOR, LTD_VERSION_MINOR);
}