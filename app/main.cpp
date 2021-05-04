#include <regex>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>

#include <ltd.h>
#include <ltd_cfg.h>

using namespace ltd;

error read_output(const char *command, std::string &out_buffer)
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
    fmt::println("-- Detecting %s - %s", name, desc);
    auto exec_ret = read_output(cmd, out);
    if (exec_ret == error::no_error) {
        std::regex e(rex);
        std::cmatch cm;

        if (std::regex_search(out.c_str(), cm, e, std::regex_constants::match_any)) {
            fmt::println("--     %s detected. %s version is %s", name, name, cm[0]);
            return error::no_error;
        }
    } 

    fmt::println("%s is not detected. Exiting...", name);
    return error::not_found;
}

void print_help(args_opt& opts)
{
    fmt::println("Usage: ltd <command> [options...]");
    fmt::println("");
    fmt::println("Available commands:");
    fmt::println("");
    fmt::println("    version       displays ltd version");
    fmt::println("    help          displays ltd help");
    fmt::println("");
    fmt::println("Options:");
    opts.print_help(4);
}

void print_version() 
{
    fmt::println("ltd version %d.%d", LTD_VERSION_MAJOR, LTD_VERSION_MINOR);
}

int main(int argc, char *argv[])
{
    std::string home_path;
    int opt_verbosity;
    args_opt flags;
    flags.init(argc, argv);

    // Bind arguments
    flags.bind(opt_verbosity, 0, 'v', "verbose", "Verbose logging.");

    if (flags.size() < 2) {
        print_help(flags); 
        return 0;
    }

    std::string command;
    error err = error::no_error;

    catch_ret(command, err) = flags.at(1);
    if(command == "version") {
        print_version();
    } else if (command == "help") {
        print_help(flags);
    } else {
        error exec_ret = error::no_error;

        fmt::println("Checking prerequisites...");
        
        // get LTD_HOME value
        auto env_home = getenv("LTD_HOME"); 
        if(env_home == NULL) {
            fmt::println("-- LTD_HOME is not set. Attempting to determine LTD_HOME.");
            
            struct passwd *pw = getpwuid(getuid());
            std::string homedir = pw->pw_dir;

            homedir += "/ltd_home/";

            if (fs::exists(homedir) == false) {
                fmt::println("--     '%s' does not exist. Exiting...", homedir);
                return -1;
            }

            home_path = homedir;
            fmt::println("--     Using '%s' as LTD_HOME...", home_path);
        } else {
            home_path = env_home;
            if (home_path.back() != '/') {
                home_path += "/";
            }
        }

        if (check_version("CMake", "the cross-platform buildsystem generator", "cmake --version", "([0-9]+\\.[0-9]+\\.[0-9]+)") != error::no_error)
            return -1;

        if (check_version("Git", "the stupid content tracker", "git --version", "([0-9]+\\.[0-9]+\\.[0-9]+)") != error::no_error)
            return -1;
    }
 
    return 0;
}