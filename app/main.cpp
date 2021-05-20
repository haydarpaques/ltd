#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>

#include <fstream>
#include <string>
#include <vector>
#include <regex>

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

void print_help(const args_opt& opts)
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

error config_lib(const std::string &home_path, const std::string &project_name, const std::string &lib_name, const std::string &path, std::string &cmake_text)
{
    // Create the section
    cmake_text += fmt::sprintln("\n# configure 'lib%s.a' target", lib_name);

    // Collect source files
    std::string src_path = home_path + project_name + "/" + path;
    std::string files;
    for (const auto& dir_entry : fs::directory_iterator(src_path))
    {
        if (fs::is_directory(dir_entry) == true)
                continue;

        if (dir_entry.path().extension() != ".cpp" &&
            dir_entry.path().extension() != ".cxx" &&
            dir_entry.path().extension() != ".cc") {
                continue;
            }

        files += path + "/" + dir_entry.path().filename().string();
        files += " ";
    }

    if (files.length() == 0)
        return error::not_found;

    cmake_text += fmt::sprintln("add_library( %s-lib %s)", lib_name, files);
    cmake_text += fmt::sprintln("target_include_directories( %s-lib PUBLIC \"${PROJECT_BINARY_DIR}\" )", lib_name);
    cmake_text += fmt::sprintln("target_include_directories( %s-lib PUBLIC \"${PROJECT_SOURCE_DIR}/inc/\" )", lib_name);
    cmake_text += fmt::sprintln("set_target_properties( %s-lib PROPERTIES OUTPUT_NAME %s )", lib_name, lib_name);
    cmake_text += "\n";

    return error::no_error;
}

error config_app(const std::string &home_path, const std::string &project_name, const std::string &app_name, const std::string &path, const std::vector<std::string> &project_libs, std::string &cmake_text)
{
    // Create the section
    cmake_text += fmt::sprintln("\n# configure '%s' executable target", app_name);

    // Collect source files
    std::string src_path = home_path + project_name + "/" + path;
    std::string files;
    for (const auto& dir_entry : fs::directory_iterator(src_path))
    {
        if (fs::is_directory(dir_entry) == true)
                continue;

        if (dir_entry.path().extension() != ".cpp" &&
            dir_entry.path().extension() != ".cxx" &&
            dir_entry.path().extension() != ".cc") {
                continue;
            }

        files += path + "/" + dir_entry.path().filename().string();
        files += " ";
    }

    if (files.length() == 0)
        return error::not_found;

    cmake_text += fmt::sprintln("add_executable( %s-exe %s)", app_name, files);
    cmake_text += fmt::sprintln("target_include_directories( %s-exe PUBLIC \"${PROJECT_BINARY_DIR}\")", app_name);
    cmake_text += fmt::sprintln("target_include_directories( %s-exe PUBLIC \"${PROJECT_SOURCE_DIR}/inc/\")", app_name);
    cmake_text += fmt::sprintln("set_target_properties( %s-exe PROPERTIES OUTPUT_NAME %s )", app_name, app_name);

    std::string local_libs;
    for (auto lib_target : project_libs) {
        local_libs += lib_target + "-lib ";
    }

    cmake_text += fmt::sprintln("target_link_libraries( %s-exe %s stdc++fs)", app_name, local_libs );
    cmake_text += "\n";

    return error::no_error;
}

bool is_project_dirty(const std::string &home_path, const std::string &project_name)
{
    // We need to generate one when the following conditions met:
    // - The file is not there
    // - The file is older than the any of the folders in the project.
    if ( fs::exists(home_path + "/" + project_name + "/CMakeLists.txt") == false )
        return true;

    auto cmake_list_ts = fs::last_write_time( home_path + "/" + project_name + "/CMakeLists.txt" );
    auto folders_ts = fs::last_write_time( home_path + "/" + project_name );

    if (folders_ts > cmake_list_ts) 
        return true;

    if (fs::exists(home_path + "/" + project_name + "/app")) {
        folders_ts = fs::last_write_time( home_path + "/" + project_name + "/app" );
        if (folders_ts > cmake_list_ts) 
            return true;
    }

    if (fs::exists(home_path + "/" + project_name + "/apps")) {
        folders_ts = fs::last_write_time( home_path + "/" + project_name + "/apps" );
        if (folders_ts > cmake_list_ts) 
            return true;
    }

    if (fs::exists(home_path + "/" + project_name + "/lib")) {
        folders_ts = fs::last_write_time( home_path + "/" + project_name + "/lib" );
        if (folders_ts > cmake_list_ts) 
            return true;
    }

    if (fs::exists(home_path + "/" + project_name + "/libs")) {
        folders_ts = fs::last_write_time( home_path + "/" + project_name + "/libs" );
        if (folders_ts > cmake_list_ts) 
            return true;
    }

    return false;    
}

error clean(const args_opt& flags, const std::string &home_path) 
{
    // Reading the project name and validating
    if (flags.size() < 3) {
        fmt::println("Expecting project name");
        print_help(flags);
        return error::invalid_argument;
    }

    auto [project_name, err0] = flags.at(2);
    if (err0 != error::no_error) {
        fmt::println("Error while retrieving project name. Exiting...");
        return error::invalid_argument;
    }

    // Check apps name
    if (project_name.find('/') != std::string::npos) {
        if (fs::exists(home_path+project_name+"app") ) {
             // Check multiple apps binaries
        }
    } else {
        if (fs::exists(home_path+project_name+"app") ) {
             // Check single app binary
        }
    }

    return error::no_error;
}

error clean(const args_opt& flags, const std::string &home_path) 
{
    // Reading the project name and validating
    if (flags.size() < 3) {
        fmt::println("Expecting project name");
        print_help(flags);
        return error::invalid_argument;
    }

    auto [project_name, err0] = flags.at(2);
    if (err0 != error::no_error) {
        fmt::println("Error while retrieving project name. Exiting...");
        return error::invalid_argument;
    }

    if (fs::exists(home_path + "caches/" + project_name))
        fs::remove_all(home_path + "caches/" + project_name);  

    return error::no_error;          
}

error build(const args_opt& flags, const std::string &home_path)
{
    std::vector<std::string> project_libs;

    // Reading the project name and validating
    if (flags.size() < 3) {
        fmt::println("Expecting project name");
        print_help(flags);
        return error::invalid_argument;
    }

    auto [project_name, err0] = flags.at(2);
    if (err0 != error::no_error) {
        fmt::println("Error while retrieving project name. Exiting...");
        return error::invalid_argument;
    }

    // Check whether we need to generate new CMakeLists.txt           
    if (is_project_dirty(home_path, project_name)) {
        std::string cmake_txt;
        cmake_txt += fmt::sprintln("cmake_minimum_required(VERSION 3.10)");
        cmake_txt += fmt::sprintln("");
        cmake_txt += fmt::sprintln("project (%s)", project_name);
        cmake_txt += fmt::sprintln("");
        cmake_txt += fmt::sprintln("set(CMAKE_CXX_STANDARD 17)");
        cmake_txt += fmt::sprintln("set(CMAKE_CXX_STANDARD_REQUIRED True)");

        // Libraries section
        // Determine whether we have a single library build or multiple
        if  (fs::exists(home_path + project_name + "/lib")) {
            fmt::println("Building 'lib%s.a'", project_name);
            config_lib(home_path, project_name, project_name, "lib", cmake_txt);
            project_libs.push_back(project_name);
        }

        if  (fs::exists(home_path + project_name + "/libs")) {
            for (const auto& dir_entry : fs::directory_iterator(home_path + project_name + "/libs")) {
                if (fs::is_directory(dir_entry) == true) {
                    std::string lib_name = dir_entry.path().filename().string();
                    std::string lib_path = "libs/" + lib_name;

                    config_lib( home_path, project_name, lib_name, lib_path, cmake_txt );
                    project_libs.push_back(lib_name);
                }
            }
        }

        // Apps section
        // Determine whether we have a single app build or multiple
        if  (fs::exists(home_path + project_name + "/app")) {
            fmt::println("Building 'app' executable", project_name);
            config_app(home_path, project_name, project_name, "app", project_libs, cmake_txt);
        }

        if  (fs::exists(home_path + project_name + "/apps")) {
            for (const auto& dir_entry : fs::directory_iterator(home_path + project_name + "/apps")) {
                if (fs::is_directory(dir_entry) == true) {
                    std::string app_name = dir_entry.path().filename().string();
                    std::string app_path = "apps/" + app_name;

                    config_app( home_path, project_name, app_name, app_path, project_libs, cmake_txt );
                }
            }
        }

        if ( fs::exists(home_path + "/" + project_name + "/CMakeLists.txt") )
            fs::remove( home_path + "/" + project_name + "/CMakeLists.txt" );

        fmt::println("-- Writing CMakeLists.txt to '%s'", home_path + project_name);

        std::ofstream out(home_path + "/" + project_name + "/CMakeLists.txt");
        out << cmake_txt << std::endl;
        out.close();
    }

    if (fs::exists(home_path + "caches/" + project_name) == false)
        fs::create_directory(home_path + "caches/" + project_name);

    fs::current_path(home_path + "caches/" + project_name);
    std::system(fmt::sprintf("cmake ../../%s .", project_name).c_str());
    std::system("cmake --build .");

    return error::no_error;
}

int main(int argc, char *argv[])
{
    std::string home_path;
    int opt_verbosity;
    args_opt flags;
    flags.init(argc, argv);

    // Bind arguments
    flags.bind(opt_verbosity, 0, 'v', "verbose", "logging verbosity.");

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

        if (command == "build") 
        {
            auto err = build(flags, home_path);

            if (err != error::no_error) {
                return -1;
            }           
        } // 'build' command
        else if (command == "clean")
        {
            auto err = clean(flags, home_path);

            if (err != error::no_error) {
                return -1;
            }
        } // 'clean' command
        else 
        {
            fmt::println("Command '%s' is not recognized...", command);
            print_help(flags);
        } 
    }
 
    return 0;
}