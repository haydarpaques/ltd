/**************************************************************************************
 * Authors:
 * 1. Benni Adham, benniadham@email.com
 * 2. Husni Fahmi, husnifahmi@outlook.com
 * Date: 2021-11-27
 * Filename: main.cpp
 * Description: ltd
 * A suite of C++ build tool and library.
 *
 * {Ltd.}'s purpose is to provide environment to help writing safe
 * and performant C++ code. It does so by providing:
 * = build tools that reduces the complexity of various C/C++
 *   build tools and toolchains;
 * = library with a framework that encourages consistent and
 *   good practices in writing safe and performant code.
 *
 **************************************************************************************/

/**************************
 * date: 2021-12-12
 * ./ltd package <project_name>
 *
 * will copy includes to ltd_home/packages/project_name/inc
 * will copy binaries exe dan .a ke folder ltd_home/packages/project_name/
 *
 * other projects will refer to these packages.
 * ./ltd import project_a project_b
 *
 * ltd will add ltd_home/package/project_b/inc to include PATH
 * ltd will add ltd_home/package/project_b to library PATH
 *
 *************************/

#include <iostream>
#include <string>
#include <algorithm>

#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

#include <ltd.h>
#include <ltd_cfg.h>

#include "utils.h"
#include "project.h"

using namespace ltd;

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s)
{
    return rtrim(ltrim(s));
}

/**
 * @brief
 * Generates config for libraries
 *
 * @details
 * A project can have a single library or multiple libraries. For single library,
 * the library will use the project name as its name. i.e libproject.a.
 *
 * For multiple libraries, it use the name of the folder under /libs where the
 * source files are located. I.e if the folder is project_name/libs/mylib01 then
 * the library will be called libmylib01.a.
 *
 * @param home_path         Path to LTD_HOME
 * @param project_name      The project name
 * @param lib_name          The library name
 * @param path              The library source path
 * @param cmake_text        The string buffer for CMake configuration script
 * @return error            returns not_found if there are no source file to build
 */
error config_lib(const std::string &home_path, const std::string &project_name, const std::string &lib_name, const std::string &path, std::string &cmake_text)
{
    // Create the section
    cmake_text += log::sprintln("\n# configure 'lib%s.a' target", lib_name);

    // Collect source files
    std::string src_path = home_path + project_name + "/" + path;
    std::string files;
    for (const auto &dir_entry : fs::directory_iterator(src_path))
    {
        if (fs::is_directory(dir_entry) == true)
            continue;

        if (dir_entry.path().extension() != ".cpp" &&
            dir_entry.path().extension() != ".cxx" &&
            dir_entry.path().extension() != ".cc")
        {
            continue;
        }

        files += path + "/" + dir_entry.path().filename().string();
        files += " ";
    }

    if (files.length() == 0)
        return error::not_found;

    cmake_text += log::sprintln("add_library( %s-lib %s)", lib_name, files);
    cmake_text += log::sprintln("target_include_directories( %s-lib PUBLIC \"${PROJECT_BINARY_DIR}\" )", lib_name);
    cmake_text += log::sprintln("target_include_directories( %s-lib PUBLIC \"${PROJECT_SOURCE_DIR}/inc/\" )", lib_name);
    cmake_text += log::sprintln("set_target_properties( %s-lib PROPERTIES OUTPUT_NAME %s )", lib_name, lib_name);
    cmake_text += "\n";

    return error::no_error;
}

/**
 * @brief
 * Generates config for app/executable
 *
 * @details
 * A project can have one or more applications or executable targets. For single app,
 * the app will use the project name as its name. i.e myproject.
 *
 * For multiple applications, it use the name of the folder under /appss where the
 * source files are located. I.e if the folder is project_name/apps/myapp01 then
 * the application will be called myapp01.
 *
 * @param home_path         Path to LTD_HOME
 * @param project_name      The name of the project
 * @param app_name          The name of the application
 * @param path              The path of the application i.e ltd_home/project/apps/myapp01
 * @param project_libs      Libraries for linking
 * @param cmake_text        The string buffer to hold the CMake configuration
 * @return error            returns not_found if there's no source file to build
 */
error config_app(const std::string &home_path, const std::string &project_name, const std::string &app_name, const std::string &path, const std::vector<std::string> &project_libs, std::string &cmake_text)
{
    // Create the section
    cmake_text += log::sprintln("\n# configure '%s' executable target", app_name);

    // Collect source files
    std::string src_path = home_path + project_name + "/" + path;
    std::string files;
    for (const auto &dir_entry : fs::directory_iterator(src_path))
    {
        if (fs::is_directory(dir_entry) == true)
            continue;

        if (dir_entry.path().extension() != ".cpp" &&
            dir_entry.path().extension() != ".cxx" &&
            dir_entry.path().extension() != ".cc")
        {
            continue;
        }

        files += path + "/" + dir_entry.path().filename().string();
        files += " ";
    }

    if (files.length() == 0)
        return error::not_found;

    std::string lib_config_file;
    lib_config_file = home_path + "/" + project_name + "/ltd-lib-config.txt";
    // log::println("config: '%s'", lib_config_file);

    if (fs::exists(lib_config_file) == true)
    {
        std::string str1;
        std::string str2;
        std::ifstream infile_lib;
        char line[5004];
        std::string ldirs_line = "";
        std::string other_lines = "";
        std::string libdir;
        std::string word1 = "";
        std::string word2 = "";
        bool ldir_found = false;

        // log::println("File %s exists.", lib_config_file);
        infile_lib.open(lib_config_file);
        while (!infile_lib.eof())
        {
            infile_lib.getline(line, 4096);
            str1 = "";
            str1.append(line);
            str2 = trim(str1);
            if (str2.length() == 0)
            {
                continue;
            }
            // std::cout << "line: " << line << std::endl;

            // stringstream class check1
            std::stringstream check1(line);

            check1 >> word1;
            if (word1.compare("LD_LDIRS") == 0)
            {
                check1 >> word2;
                if (word2.compare("=") == 0)
                {
                    while (check1 >> libdir)
                    {
                        // std::cout << "libdir: " << libdir << std::endl;
                        cmake_text += log::sprintln("link_directories(%s)", libdir);
                    }
                    break;
                }
            }
        }
        infile_lib.close();
    }

    cmake_text += log::sprintln("");
    // add link directories
    // cmake_text += log::sprintln("link_directories(%s)", opt_ldirs);

    cmake_text += log::sprintln("add_executable( %s-exe %s)", app_name, files);
    cmake_text += log::sprintln("target_include_directories( %s-exe PUBLIC \"${PROJECT_BINARY_DIR}\")", app_name);
    cmake_text += log::sprintln("target_include_directories( %s-exe PUBLIC \"${PROJECT_SOURCE_DIR}/inc/\")", app_name);
    cmake_text += log::sprintln("set_target_properties( %s-exe PROPERTIES OUTPUT_NAME %s )", app_name, app_name);

    std::string local_libs;
    for (auto lib_target : project_libs)
    {
        local_libs += lib_target + "-lib ";
    }

    cmake_text += log::sprintln("target_link_libraries( %s-exe %s stdc++fs)", app_name, local_libs);

    if (fs::exists(lib_config_file) == true)
    {
        std::string str1;
        std::string str2;
        std::ifstream infile_lib;
        char line[2056];
        std::string libs_line = "";
        std::string other_lines = "";
        std::string libname;
        std::string word1 = "";
        std::string word2 = "";
        bool lib_found = false;

        // log::println("File %s exists.", lib_config_file);
        infile_lib.open(lib_config_file);
        while (!infile_lib.eof())
        {
            infile_lib.getline(line, 2048);
            str1 = "";
            str1.append(line);
            str2 = trim(str1);
            if (str2.length() == 0)
            {
                continue;
            }
            // std::cout << "line: " << line << std::endl;

            // stringstream class check1
            std::stringstream check1(line);

            check1 >> word1;
            if (word1.compare("LD_LIBS") == 0)
            {
                check1 >> word2;
                if (word2.compare("=") == 0)
                {
                    while (check1 >> libname)
                    {
                        // std::cout << "libname: " << libname << std::endl;
                        cmake_text += log::sprintln("target_link_libraries( %s-exe %s)", app_name, libname);
                    }
                    break;
                }
            }
        }
        infile_lib.close();
    }

    if (fs::exists(lib_config_file) == true)
    {
        std::string str1;
        std::string str2;
        std::ifstream infile_lib;
        char line[5004];
        std::string files_line = "";
        std::string other_lines = "";
        std::string fname;
        std::string word1 = "";
        std::string word2 = "";
        bool file_found = false;

        // log::println("File %s exists.", lib_config_file);
        infile_lib.open(lib_config_file);
        while (!infile_lib.eof())
        {
            infile_lib.getline(line, 4096);
            str1 = "";
            str1.append(line);
            str2 = trim(str1);
            if (str2.length() == 0)
            {
                continue;
            }
            // std::cout << "line: " << line << std::endl;

            // stringstream class check1
            std::stringstream check1(line);

            check1 >> word1;
            if (word1.compare("COPY_FILES") == 0)
            {
                check1 >> word2;
                if (word2.compare("=") == 0)
                {
                    while (check1 >> fname)
                    {
                        // std::cout << "libdir: " << libdir << std::endl;
                        cmake_text += log::sprintln("configure_file(%s %s COPYONLY)", fname, fname);
                    }
                    break;
                }
            }
        }
        infile_lib.close();
    }

    // cmake_text += log::sprintln("target_link_libraries( %s-exe zmq)", app_name);
    // cmake_text += log::sprintln("target_link_libraries( %s-exe %s)", app_name, local_libs);
    // cmake_text += log::sprintln("target_link_libraries(ltd_sandbox zmq)");
    cmake_text += "\n";

    return error::no_error;
}

/**
 * @brief
 * Generates config for test units
 *
 * @details
 * The ltd framework collaborate features on CMake test and its own framework.
 * It provides test_unit class that would let the user to create test unit and
 * the test unit will communicate with CMake's test framework through stdout.
 *
 * I.e. to use the test framework, one might use it as follow:
 *
 * ```
 * #include <ltd.h>
 *
 * using namespace ltd;
 *
 * auto main(int argc, char** argv) -> int
 * {
 *     test_unit tu;
 *
 *     tu.test([&tu](){
 *         tu.expect(true, "Expected true");
 *     });
 *
 *     tu.test([&tu](){
 *         tu.expect(true, "Expected true");
 *     });
 *
 *     tu.test([&tu](){
 *         tu.expect(true, "Expected true");
 *     });
 *
 *     tu.run(argc, argv);
 *
 *     return 0;
 * }
 * ```
 *
 * Each test correlate with 1 test in CMake's test. The number of the test case
 * will be determined by calling the test binary. When the test binary called without
 * arguments, it will return the number of tests available. To run the test, specify
 * the test id in the cli argument.
 *
 * ```
 * >./mytest 1
 * ```
 *
 * Test id starts from 0. In this example, the program will run the second test case.
 *
 * When test command runs, ltd will call ctest with -VV as parameter argument in
 * the project cache path.
 *
 * @param home_path         Path to LTD_HOME
 * @param project_name      The name of the project
 * @param project_libs      Libraries for linking
 * @param cmake_text        The string buffer to hold the CMake configuration
 * @return error            returns not_found if there's no source file to build
 */
error config_test(const std::string &home_path, const std::string &project_name, const std::vector<std::string> &project_libs, std::string &cmake_text)
{
    cmake_text += log::sprintln("\n# configure test executable target");

    // [TODO: check project name before execute build... this command crash because the project name is wrong ]
    // ./ltd build ltd_sanbox << ERROR!!!

    // Collect source files
    std::string src_path = home_path + project_name + "/tests";
    std::vector<std::string> files;
    for (const auto &dir_entry : fs::directory_iterator(src_path))
    {
        log::println("%s", dir_entry.path().string());
        if (fs::is_directory(dir_entry) == true)
            continue;

        if (dir_entry.path().extension() != ".cpp" &&
            dir_entry.path().extension() != ".cxx" &&
            dir_entry.path().extension() != ".cc")
        {
            continue;
        }

        auto bin_name = dir_entry.path().filename().replace_extension("").string();

        cmake_text += log::sprintln("add_executable( %s-test tests/%s)", bin_name, dir_entry.path().filename().string());
        cmake_text += log::sprintln("target_include_directories( %s-test PUBLIC \"${PROJECT_BINARY_DIR}\")", bin_name);
        cmake_text += log::sprintln("target_include_directories( %s-test PUBLIC \"${PROJECT_SOURCE_DIR}/inc/\")", bin_name);
        cmake_text += log::sprintln("target_include_directories( %s-test PUBLIC \"%s/ltd/inc/\")", bin_name, home_path);
        cmake_text += log::sprintln("set_target_properties( %s-test PROPERTIES OUTPUT_NAME %s )", bin_name, bin_name);

        std::string local_libs;
        for (auto lib_target : project_libs)
        {
            local_libs += lib_target + "-lib ";
        }

        cmake_text += log::sprintln("target_link_libraries( %s-test %s stdc++fs ltd)", bin_name, local_libs);
        cmake_text += log::sprintln("target_link_directories( %s-test PUBLIC \"%s/caches/ltd/\")", bin_name, home_path);
        cmake_text += "\n";

        files.push_back(bin_name);
    }

    if (files.size() == 0)
        return error::not_found;

    cmake_text += log::sprintln("# Test section");
    cmake_text += log::sprintln("enable_testing()");
    cmake_text += log::sprintln("");
    cmake_text += log::sprintln("# define a function to simplify adding tests");
    cmake_text += log::sprintln("function(do_test target arg result)");
    cmake_text += log::sprintln("add_test(NAME Comp${arg} COMMAND ${target} ${arg})");
    cmake_text += log::sprintln("set_tests_properties(Comp${arg}");
    cmake_text += log::sprintln("    PROPERTIES PASS_REGULAR_EXPRESSION ${result}");
    cmake_text += log::sprintln("    )");
    cmake_text += log::sprintln("endfunction(do_test)");
    cmake_text += log::sprintln("");
    cmake_text += log::sprintln("function(config_test_bin testbin)");
    cmake_text += log::sprintln("    execute_process(COMMAND \"./${testbin}\"");
    cmake_text += log::sprintln("        OUTPUT_VARIABLE out1)");
    cmake_text += log::sprintln("    math(EXPR LOOP_STOP \"${out1} - 1\")");
    cmake_text += log::sprintln("");
    cmake_text += log::sprintln("    foreach(X RANGE 0 ${LOOP_STOP})");
    cmake_text += log::sprintln("        do_test(${testbin} ${X} \"-ok-\")");
    cmake_text += log::sprintln("    endforeach()");
    cmake_text += log::sprintln("endfunction(config_test_bin)");

    for (auto test_bin : files)
    {
        cmake_text += log::sprintln("config_test_bin(%s)", test_bin);
    }

    return error::no_error;
}

/**
 * @brief
 * Determines whether the rebuild is necessary
 *
 * @details
 * Ltd will check whether the project is dirty. If it is dirty then a rebuild
 * is needed.
 *
 * To determine whether a project dirty or not, the following rules are used:
 * - If there is no CMakeLists.txt file in the project folder
 * - If the project folder timestamp is younger than the CMake list file.
 * - If the apps folder timestamp is younger than the CMake list file.
 * - If the libraries folder timestamp is younger than the CMake list file.
 *
 * @param home_path         Path to LTD_HOME
 * @param project_name      The name of the project
 * @return true             If the project needs a rebuild
 * @return false            If the project binaries are at the latest
 */
bool is_project_dirty(const std::string &home_path, const std::string &project_name)
{
    // We need to generate one when the following conditions met:
    // - The file is not there
    // - The file is older than the any of the folders in the project.
    if (fs::exists(home_path + "/" + project_name + "/CMakeLists.txt") == false)
        return true;

    auto cmake_list_ts = fs::last_write_time(home_path + "/" + project_name + "/CMakeLists.txt");
    auto folders_ts = fs::last_write_time(home_path + "/" + project_name);

    std::string lib_config_file;
    lib_config_file = home_path + "/" + project_name + "/ltd-lib-config.txt";
    if (fs::exists(lib_config_file) == true)
    {
        auto lib_config_ts = fs::last_write_time(lib_config_file);
        // ltd-lib-config.txt is newer than CMakeLists.txt
        if (lib_config_ts > cmake_list_ts)
            return true;
    }

    if (folders_ts > cmake_list_ts)
        return true;

    if (fs::exists(home_path + "/" + project_name + "/app"))
    {
        folders_ts = fs::last_write_time(home_path + "/" + project_name + "/app");
        if (folders_ts > cmake_list_ts)
            return true;
    }

    if (fs::exists(home_path + "/" + project_name + "/apps"))
    {
        folders_ts = fs::last_write_time(home_path + "/" + project_name + "/apps");
        if (folders_ts > cmake_list_ts)
            return true;
    }

    if (fs::exists(home_path + "/" + project_name + "/lib"))
    {
        folders_ts = fs::last_write_time(home_path + "/" + project_name + "/lib");
        if (folders_ts > cmake_list_ts)
            return true;
    }

    if (fs::exists(home_path + "/" + project_name + "/libs"))
    {
        folders_ts = fs::last_write_time(home_path + "/" + project_name + "/libs");
        if (folders_ts > cmake_list_ts)
            return true;
    }

    return false;
}

/**
 * @brief
 * Runs a project binary target
 *
 * @details
 * Run a project. The command format is as follow:
 * - single executable
 * ```
 * ltd run project_name
 * ```
 * - multiple executables
 * ```
 * ltd run project_name/app_name
 * ```
 *
 * @param home_path         Path to LTD_HOME
 * @param project_name      The name of the project
 * @return error            Returns `invalid_arguments` if project name is not specify or invalid
 *                          Returns `not_found` if a target is not found
 */
error run(const cli_arguments &flags, const std::string &home_path)
{
    // Reading the project name and validating
    if (flags.size() < 3)
    {
        log::println("Expecting project name");
        print_help();
        flags.print_help(4);
        return error::invalid_argument;
    }

    auto [raw_arg, err0] = flags.at(2);
    if (err0 != error::no_error)
    {
        log::println("Error while retrieving target name. Exiting...");
        return error::invalid_argument;
    }

    std::string target = raw_arg;

    // Check apps name
    auto pos = target.find('/');
    if (pos != std::string::npos)
    {
        auto project_name = target.substr(0, pos);
        auto app_name = target.substr(pos + 1);

        if (fs::exists(home_path + project_name + "/apps/" + app_name))
        {
            auto cmd_target = home_path + "caches/" + project_name + "/" + app_name;
            if (fs::exists(cmd_target))
            {
                std::system(cmd_target.c_str());
                return error::no_error;
            }
            else
            {
                log::println("Target '%s' does not exist...", cmd_target);
                return error::not_found;
            }
        }
        else
        {
            log::println("%s", home_path + project_name + "apps/" + app_name);
            log::println("Application '%s' for project '%s' does not exist", app_name, project_name);
            return error::not_found;
        }
    }
    else
    {
        if (fs::exists(home_path + target + "/app"))
        {
            auto cmd_target = home_path + "caches" + target + "/" + target;
            if (fs::exists(cmd_target))
            {
                std::system(cmd_target.c_str());
                return error::no_error;
            }
            else
            {
                log::println("Target '%s' does not exist...", cmd_target);
                return error::not_found;
            }
        }
        else
        {
            log::println("Application '%s' does not exist", target);
            return error::not_found;
        }
    }

    return error::no_error;
}

/**
 * @brief
 * Cleans a project
 *
 * @details
 * Delete all temporary and binary output files.
 * Command format:
 * ```
 * ltd clean project_name
 * ```
 *
 * @param flags             `cli_arguments` containing the cli argument flags
 * @param home_path         Path to LTD_HOME
 * @return error            Returns invalid arguments when project name is not found or invalid
 */
error clean(const cli_arguments &flags, const std::string &home_path)
{
    // Reading the project name and validating
    if (flags.size() < 3)
    {
        log::println("Expecting project name");
        print_help();
        flags.print_help(4);
        return error::invalid_argument;
    }

    auto [project_name, err0] = flags.at(2);
    if (err0 != error::no_error)
    {
        log::println("Error while retrieving project name. Exiting...");
        return error::invalid_argument;
    }

    if (fs::exists(home_path + "caches/" + project_name))
        fs::remove_all(home_path + "caches/" + project_name);

    return error::no_error;
}

/**
 * @brief
 * Runs a test unit for a project.
 *
 * @details
 * The ltd framework collaborate features on CMake test and its own framework.
 * It provides test_unit class that would let the user to create test unit and
 * the test unit will communicate with CMake's test framework through stdout.
 *
 * I.e. to use the test framework, one might use it as follow:
 *
 * ```
 * #include <ltd.h>
 *
 * using namespace ltd;
 *
 * auto main(int argc, char** argv) -> int
 * {
 *     test_unit tu;
 *
 *     tu.test([&tu](){
 *         tu.expect(true, "Expected true");
 *     });
 *
 *     tu.test([&tu](){
 *         tu.expect(true, "Expected true");
 *     });
 *
 *     tu.test([&tu](){
 *         tu.expect(true, "Expected true");
 *     });
 *
 *     tu.run(argc, argv);
 *
 *     return 0;
 * }
 * ```
 *
 * Each test correlate with 1 test in CMake's test. The number of the test case
 * will be determined by calling the test binary. When the test binary called without
 * arguments, it will return the number of tests available. To run the test, specify
 * the test id in the cli argument.
 *
 * ```
 * >./mytest 1
 * ```
 *
 * Test id starts from 0. In this example, the program will run the second test case.
 *
 * When test command runs, ltd will call ctest with -VV as parameter argument in
 * the project cache path.
 *
 * @param flags             `cli_arguments` containing the cli argument flags
 * @param home_path         Path to LTD_HOME
 * @return error            Returns invalid arguments when project name is not found or invalid
 */
error test(const cli_arguments &flags, const std::string &home_path)
{
    // Reading the project name and validating
    if (flags.size() < 3)
    {
        log::println("Expecting project name");
        print_help();
        flags.print_help(4);
        return error::invalid_argument;
    }

    auto [project_name, err0] = flags.at(2);
    if (err0 != error::no_error)
    {
        log::println("Error while retrieving project name. Exiting...");
        return error::invalid_argument;
    }

    if (fs::exists(home_path + "caches/" + project_name))
    {
        fs::current_path(home_path + "caches/" + project_name);
        std::system("ctest -VV"); // TODO: control the verbosity level
    }
    else
    {
        log::println("Cannot find files for '%s'", project_name);
        return error::invalid_argument;
    }

    return error::no_error;
}

/**
 * @brief
 * Builds a project.
 *
 * @details
 * Build binaries in a project. The binary can be an executable or a static library.
 *
 * When the project folder has 'libs' folder, multiple libraries build mode is enabled.
 * The toolds will iterate folders under the 'libs' folder and create libfolder_name.a as an output.
 *
 * In the project has lib folder, it will engage the single library mode. Meaning,
 * the system will compile all files under 'lib' folder and creates libproject_name.a as an output.
 *
 * @param flags             `cli_arguments` containing the cli argument flags
 * @param home_path         Path to LTD_HOME
 * @return error            Returns invalid arguments when project name is not found or invalid
 */
error build(const cli_arguments &flags, const std::string &home_path)
{
    std::vector<std::string> project_libs;

    // Reading the project name and validating
    if (flags.size() < 3)
    {
        log::println("Expecting project name");
        print_help();
        flags.print_help(4);
        return error::invalid_argument;
    }

    auto [project_name, err0] = flags.at(2);
    if (err0 != error::no_error)
    {
        log::println("Error while retrieving project name. Exiting...");
        return error::invalid_argument;
    }

    // Check whether the project exist!
    if (!fs::exists(home_path + project_name))
    {
        log::println("Error project '%s' not found. Exiting...", project_name);
        return error::not_found;
    }

    // Check whether we need to generate new CMakeLists.txt
    if (is_project_dirty(home_path, project_name))
    // if (true)
    {
        std::string cmake_txt;
        cmake_txt += log::sprintln("cmake_minimum_required(VERSION 3.10)");
        cmake_txt += log::sprintln("");
        cmake_txt += log::sprintln("project (%s)", project_name);
        cmake_txt += log::sprintln("");
        cmake_txt += log::sprintln("set(CMAKE_CXX_STANDARD 17)");
        cmake_txt += log::sprintln("set(CMAKE_CXX_STANDARD_REQUIRED True)");

        // Libraries section
        // Determine whether we have a single library build or multiple
        if (fs::exists(home_path + project_name + "/lib"))
        {
            log::println("Building 'lib%s.a'", project_name);
            config_lib(home_path, project_name, project_name, "lib", cmake_txt);
            project_libs.push_back(project_name);
        }

        if (fs::exists(home_path + project_name + "/libs"))
        {
            for (const auto &dir_entry : fs::directory_iterator(home_path + project_name + "/libs"))
            {
                if (fs::is_directory(dir_entry) == true)
                {
                    std::string lib_name = dir_entry.path().filename().string();
                    std::string lib_path = "libs/" + lib_name;

                    config_lib(home_path, project_name, lib_name, lib_path, cmake_txt);
                    project_libs.push_back(lib_name);
                }
            }
        }

        // Apps section
        // Determine whether we have a single app build or multiple
        if (fs::exists(home_path + project_name + "/app"))
        {
            log::println("Building 'app' executable", project_name);
            config_app(home_path, project_name, project_name, "app",
                       project_libs, cmake_txt);
        }

        if (fs::exists(home_path + project_name + "/apps"))
        {
            for (const auto &dir_entry : fs::directory_iterator(home_path + project_name + "/apps"))
            {
                if (fs::is_directory(dir_entry) == true)
                {
                    std::string app_name = dir_entry.path().filename().string();
                    std::string app_path = "apps/" + app_name;

                    config_app(home_path, project_name, app_name, app_path, project_libs, cmake_txt);
                }
            }
        }

        // Test section
        // Builds test cases
        config_test(home_path, project_name, project_libs, cmake_txt);

        if (fs::exists(home_path + "/" + project_name + "/CMakeLists.txt"))
            fs::remove(home_path + "/" + project_name + "/CMakeLists.txt");

        // cmake_txt += log::sprintln("set(CMAKE_CXX_STANDARD_REQUIRED True)");

        log::println("-- Writing CMakeLists.txt to '%s'", home_path + project_name);

        std::ofstream out(home_path + "/" + project_name + "/CMakeLists.txt");
        out << cmake_txt << std::endl;
        out.close();
        // std::cout << "cmake_txt: " << cmake_txt << std::endl;
    }

    // Create directory if it does not exist
    if (fs::exists(home_path + "caches/" + project_name) == false)
        fs::create_directory(home_path + "caches/" + project_name);

    fs::current_path(home_path + "caches/" + project_name);
    std::system(log::sprintf("cmake ../../%s .", project_name).c_str());
    std::system("cmake --build .");
    log::println("-- Running second pass for test generation");
    std::system(log::sprintf("cmake ../../%s .", project_name).c_str());

    return error::no_error;
}

error config_opt_libs(const cli_arguments &flags, const std::string &home_path,
                      std::string opt_libs)
{
    // std::vector<std::string> project_libs;
    std::string lib_config_file;

    // Reading the project name and validating
    if (flags.size() < 3)
    {
        log::println("Expecting project name");
        print_help();
        flags.print_help(4);
        return error::invalid_argument;
    }

    auto [project_name, err0] = flags.at(2);
    if (err0 != error::no_error)
    {
        log::println("Error while retrieving project name. Exiting...");
        return error::invalid_argument;
    }

    // Check whether the project exist!
    if (!fs::exists(home_path + project_name))
    {
        log::println("Error project '%s' not found. Exiting...", project_name);
        return error::not_found;
    }

    // log::println("opt_libs '%s'...", opt_libs);

    lib_config_file = home_path + "/" + project_name + "/ltd-lib-config.txt";
    // log::println("config: '%s'", lib_config_file);

    if (fs::exists(lib_config_file) == true)
    {
        std::string str1;
        std::string str2;
        std::ifstream infile_lib;
        char line[2056];
        std::string libs_line = "";
        std::string other_lines = "";
        std::string word = "";
        std::string word1 = "";
        std::string word2 = "";
        bool lib_found = false;

        // log::println("File %s exists.", lib_config_file);
        infile_lib.open(lib_config_file);
        while (!infile_lib.eof())
        {
            infile_lib.getline(line, 2048);
            str1 = "";
            str1.append(line);
            str2 = trim(str1);
            if (str2.length() == 0)
            {
                continue;
            }
            // std::cout << "line: " << line << std::endl;

            // stringstream class check1
            std::stringstream check1(line);

            check1 >> word1;
            if (word1.compare("LD_LIBS") == 0)
            {
                check1 >> word2;
                // std::cout << "word2: " << word2 << std::endl;
                // std::string libs_line(line);
                libs_line.append(line);
                if (word2.compare("=") == 0)
                {
                    while (check1 >> word)
                    {
                        // std::cout << "word: " << word << std::endl;
                        // std::cout << "opt_libs: " << opt_libs << std::endl;
                        if (word.compare(opt_libs) == 0)
                        {
                            // std::cout << "opt_libs found." << std::endl;
                            lib_found = true;
                            break;
                        }
                    }
                }
                if (lib_found == false)
                {
                    libs_line = libs_line + " " + opt_libs;
                    // std::cout << "1. libs_line: " << libs_line << std::endl;
                }
            }
            else
            {
                other_lines.append(str2);
                other_lines.append("\n");
                // std::cout << "other_lines: " << other_lines << std::endl;
            }
        }

        infile_lib.close();
        if (lib_found == false)
        {
            std::ofstream out(lib_config_file);
            // std::cout << "2. libs_line: " << libs_line << std::endl;
            if (libs_line.length() != 0)
            {
                out << libs_line << std::endl;
            }
            else
            {
                out << "LD_LIBS = " << opt_libs << std::endl;
            }
            other_lines = trim(other_lines);
            if (other_lines.length() > 0)
            {
                out << other_lines << std::endl;
            }
            out.close();
            log::println("Library '%s' added to '%s'", opt_libs, lib_config_file);
        }
        else
        {
            log::println("Library '%s' already found in '%s'.", opt_libs, lib_config_file);
        }
    }
    else
    {
        // std::cout << "File " << lib_config_file << " does NOT exist." << std::endl;
        std::ofstream out(lib_config_file);
        out << "LD_LIBS = " << opt_libs << std::endl;
        out.close();
        log::println("Library '%s' added to '%s'", opt_libs, lib_config_file);
    }
    return error::no_error;
} // config_opt_libs()

error config_opt_ldirs(const cli_arguments &flags, const std::string &home_path,
                       std::string opt_ldirs)
{
    std::string lib_config_file;

    // Reading the project name and validating
    if (flags.size() < 3)
    {
        log::println("Expecting project name");
        print_help();
        flags.print_help(4);
        return error::invalid_argument;
    }

    auto [project_name, err0] = flags.at(2);
    if (err0 != error::no_error)
    {
        log::println("Error while retrieving project name. Exiting...");
        return error::invalid_argument;
    }

    // Check whether the project exist!
    if (!fs::exists(home_path + project_name))
    {
        log::println("Error project '%s' not found. Exiting...", project_name);
        return error::not_found;
    }

    log::println("opt_ldirs '%s'...", opt_ldirs);

    lib_config_file = home_path + "/" + project_name + "/ltd-lib-config.txt";
    // log::println("config: '%s'", lib_config_file);

    if (fs::exists(lib_config_file) == true)
    {
        std::string str1;
        std::string str2;
        std::ifstream infile_lib;
        char line[5004];
        std::string ldirs_line = "";
        std::string other_lines = "";
        std::string word = "";
        std::string word1 = "";
        std::string word2 = "";
        bool ldir_found = false;

        // log::println("File %s exists.", lib_config_file);
        infile_lib.open(lib_config_file);
        while (!infile_lib.eof())
        {
            infile_lib.getline(line, 4096);
            str1 = "";
            str1.append(line);
            str2 = trim(str1);
            if (str2.length() == 0)
            {
                continue;
            }
            // std::cout << "line: " << line << std::endl;

            // stringstream class check1
            std::stringstream check1(line);

            check1 >> word1;
            if (word1.compare("LD_LDIRS") == 0)
            {
                check1 >> word2;
                // std::cout << "word2: " << word2 << std::endl;
                // std::string libs_line(line);
                ldirs_line.append(line);
                if (word2.compare("=") == 0)
                {
                    while (check1 >> word)
                    {
                        // std::cout << "word: " << word << std::endl;
                        // std::cout << "opt_libs: " << opt_libs << std::endl;
                        if (word.compare(opt_ldirs) == 0)
                        {
                            // std::cout << "opt_libs found." << std::endl;
                            ldir_found = true;
                            break;
                        }
                    }
                }
                if (ldir_found == false)
                {
                    ldirs_line = ldirs_line + " " + opt_ldirs;
                    // std::cout << "1. libs_line: " << libs_line << std::endl;
                }
            }
            else
            {
                other_lines.append(str2);
                other_lines.append("\n");
                // std::cout << "other_lines: " << other_lines << std::endl;
            }
        }

        infile_lib.close();
        if (ldir_found == false)
        {
            std::ofstream out(lib_config_file);
            other_lines = trim(other_lines);
            if (other_lines.length() > 0)
            {
                out << other_lines << std::endl;
            }

            // std::cout << "2. libs_line: " << libs_line << std::endl;
            if (ldirs_line.length() != 0)
            {
                out << ldirs_line << std::endl;
            }
            else
            {
                out << "LD_LDIRS = " << opt_ldirs << std::endl;
            }
            out.close();
            log::println("Library directory '%s' added to '%s'", opt_ldirs, lib_config_file);
        }
        else
        {
            log::println("Library directory '%s' already found in '%s'.", opt_ldirs, lib_config_file);
        }
    }
    else
    {
        // std::cout << "File " << lib_config_file << " does NOT exist." << std::endl;
        std::ofstream out(lib_config_file);
        out << "LD_LDIRS = " << opt_ldirs << std::endl;
        out.close();
        log::println("Library directory '%s' added to '%s'", opt_ldirs, lib_config_file);
    }
    return error::no_error;
} // config_opt_ldirs()

error config_opt_files(const cli_arguments &flags, const std::string &home_path,
                       std::string opt_files)
{
    std::string lib_config_file;

    // Reading the project name and validating
    if (flags.size() < 3)
    {
        log::println("Expecting project name");
        print_help();
        flags.print_help(4);
        return error::invalid_argument;
    }

    auto [project_name, err0] = flags.at(2);
    if (err0 != error::no_error)
    {
        log::println("Error while retrieving project name. Exiting...");
        return error::invalid_argument;
    }

    // Check whether the project exist!
    if (!fs::exists(home_path + project_name))
    {
        log::println("Error project '%s' not found. Exiting...", project_name);
        return error::not_found;
    }

    log::println("opt_files '%s'...", opt_files);

    lib_config_file = home_path + "/" + project_name + "/ltd-lib-config.txt";
    // log::println("config: '%s'", lib_config_file);

    if (fs::exists(lib_config_file) == true)
    {
        std::string str1;
        std::string str2;
        std::ifstream infile_lib;
        char line[5004];
        std::string files_line = "";
        std::string other_lines = "";
        std::string word = "";
        std::string word1 = "";
        std::string word2 = "";
        bool file_found = false;

        // log::println("File %s exists.", lib_config_file);
        infile_lib.open(lib_config_file);
        while (!infile_lib.eof())
        {
            infile_lib.getline(line, 4096);
            str1 = "";
            str1.append(line);
            str2 = trim(str1);
            if (str2.length() == 0)
            {
                continue;
            }
            // std::cout << "line: " << line << std::endl;

            // stringstream class check1
            std::stringstream check1(line);

            check1 >> word1;
            if (word1.compare("COPY_FILES") == 0)
            {
                check1 >> word2;
                // std::cout << "word2: " << word2 << std::endl;
                // std::string libs_line(line);
                files_line.append(line);
                if (word2.compare("=") == 0)
                {
                    while (check1 >> word)
                    {
                        // std::cout << "word: " << word << std::endl;
                        // std::cout << "opt_files: " << opt_files << std::endl;
                        if (word.compare(opt_files) == 0)
                        {
                            // std::cout << "opt_libs found." << std::endl;
                            file_found = true;
                            break;
                        }
                    }
                }
                if (file_found == false)
                {
                    files_line = files_line + " " + opt_files;
                    // std::cout << "1. files_line: " << files_line << std::endl;
                }
            }
            else
            {
                other_lines.append(str2);
                other_lines.append("\n");
                // std::cout << "other_lines: " << other_lines << std::endl;
            }
        }

        infile_lib.close();
        if (file_found == false)
        {
            std::ofstream out(lib_config_file);
            other_lines = trim(other_lines);
            if (other_lines.length() > 0)
            {
                out << other_lines << std::endl;
            }

            // std::cout << "2. files_line: " << files_line << std::endl;
            if (files_line.length() != 0)
            {
                out << files_line << std::endl;
            }
            else
            {
                out << "COPY_FILES = " << opt_files << std::endl;
            }
            out.close();
            log::println("Library directory '%s' added to '%s'", opt_files, lib_config_file);
        }
        else
        {
            log::println("Library directory '%s' already found in '%s'.", opt_files, lib_config_file);
        }
    }
    else
    {
        // std::cout << "File " << lib_config_file << " does NOT exist." << std::endl;
        std::ofstream out(lib_config_file);
        out << "COPY_FILES = " << opt_files << std::endl;
        out.close();
        log::println("Library directory '%s' added to '%s'", opt_files, lib_config_file);
    }
    return error::no_error;
} // config_opt_files()

error package_project(const cli_arguments &flags, const std::string &home_path)
{
    std::string lib_config_file;

    // Reading the project name and validating
    if (flags.size() < 3)
    {
        log::println("Expecting project name");
        print_help();
        flags.print_help(4);
        return error::invalid_argument;
    }

    auto [project_name, err0] = flags.at(2);
    if (err0 != error::no_error)
    {
        log::println("Error while retrieving project name. Exiting...");
        return error::invalid_argument;
    }

    // Check whether the project exist!
    if (!fs::exists(home_path + project_name))
    {
        log::println("Error project '%s' not found. Exiting...", project_name);
        return error::not_found;
    }

    log::println("./ltd package %s...", project_name);

    // lib_config_file = home_path + "/" + project_name + "/ltd-lib-config.txt";
    // log::println("config: '%s'", lib_config_file);

    std::string project_inc_path = home_path + "/" + project_name + "/inc";

    if (std::experimental::filesystem::exists(project_inc_path) == false)
    {
        log::println("Error: project include directory %s does not exist.",
                     project_inc_path);
    }
    else
    {
        std::string packages_inc_path = home_path + "/packages" + "/" +
                                        project_name + "/inc";
        std::experimental::filesystem::copy_options co =
            std::experimental::filesystem::copy_options::overwrite_existing |
            std::experimental::filesystem::copy_options::recursive;
        if (std::experimental::filesystem::exists(packages_inc_path) == false)
        {
            if (std::experimental::filesystem::create_directories(packages_inc_path))
            {
                std::experimental::filesystem::copy(project_inc_path, packages_inc_path, co);
                log::println("copied files from %s to %s",
                             project_inc_path, packages_inc_path);
            }
            else
            {
                log::println("Error in creating directory %s.", packages_inc_path);
            }
        }
        else
        {
            log::println("Directory %s already exists.", packages_inc_path);
            std::experimental::filesystem::copy(project_inc_path, packages_inc_path, co);
            log::println("copied files from %s to %s",
                         project_inc_path, packages_inc_path);
        }
    }

    return error::no_error;
} // error package_project()

auto main(int argc, char *argv[]) -> int
{

    std::string home_path;
    std::string opt_libs;
    std::string opt_ldirs;
    std::string opt_files;
    std::string opt_imports;

    int opt_verbose;
    cli_arguments flags;

    // Bind arguments
    flags.bind(&opt_libs, 'l', "libraries", "input system libraries.");
    flags.bind(&opt_ldirs, 'L', "libdirs", "library directories.");
    flags.bind(&opt_files, 'f', "files", "copy files to binary directory.");
    flags.bind(&opt_imports, 'i', "imports ", "import ltd projects.");
    flags.bind(&opt_verbose, 'v', "verbose", "logging verbosity.");
    flags.parse(argc, argv);

    if (flags.size() < 2)
    {
        print_help();
        flags.print_help(4);
        return 0;
    }

    std::string command;
    error err = error::no_error;

    catch_ret(command, err) = flags.at(1);
    if (command == "version")
    {
        print_version();
    }
    else if (command == "help")
    {
        print_help();
    }
    else
    {
        log::println("Checking prerequisites...");

        // get LTD_HOME value
        auto env_home = getenv("LTD_HOME");
        if (env_home == NULL)
        {
            log::println("-- LTD_HOME is not set. Attempting to determine LTD_HOME.");

            struct passwd *pw = getpwuid(getuid());
            std::string homedir = pw->pw_dir;

            homedir += "/ltd_home/";

            if (fs::exists(homedir) == false)
            {
                log::println("--     '%s' does not exist. Exiting...", homedir);
                return -1;
            }

            home_path = homedir;
            log::println("--     Using '%s' as LTD_HOME...", home_path);
        }
        else
        {
            home_path = env_home;
            if (home_path.back() != '/')
            {
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

            if (err != error::no_error)
            {
                return -1;
            }
        } // 'build' command
        else if (command == "config")
        {
            ltd::error err("");
            if (opt_libs.length() > 0)
            {
                err = config_opt_libs(flags, home_path, opt_libs);
            }
            else if (opt_ldirs.length() > 0)
            {
                std::cout << "ldirs" << std::endl;
                err = config_opt_ldirs(flags, home_path, opt_ldirs);
            }
            else if (opt_files.length() > 0)
            {
                std::cout << "files" << std::endl;
                err = config_opt_files(flags, home_path, opt_files);
            }

            if (err != error::no_error)
            {
                return -1;
            }
        } // 'config' command
        else if (command == "package")
        {
            ltd::error err("");
            err = package_project(flags, home_path);
        }
        else if (command == "clean")
        {
            auto err = clean(flags, home_path);

            if (err != error::no_error)
            {
                return -1;
            }
        } // 'clean' command
        else if (command == "run")
        {
            auto err = run(flags, home_path);

            if (err != error::no_error)
            {
                return -1;
            }
        }
        else if (command == "test")
        {
            auto err = test(flags, home_path);

            if (err != error::no_error)
            {
                return -1;
            }
        }
        else
        {
            log::println("Command '%s' is not recognized...", command);
            print_help();
            flags.print_help(4);
        }
    }

    return 0;
}
