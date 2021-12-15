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

#include "utils.h"

#include <ltd_cfg.h>

error exec_and_read(const char *command, std::string &out_buffer)
{
    FILE *fp;
    const int sizebuf = 1024;
    char buff[sizebuf];

    if ((fp = popen(command, "r")) == NULL)
        return error::invalid_operation;

    while (fgets(buff, sizeof(buff), fp))
        out_buffer += buff;

    pclose(fp);

    return error::no_error;
}

error check_version(const char *name, const char *desc, const char *cmd, const char *rex)
{
    std::string out;
    log::println("-- Detecting %s - %s", name, desc);
    auto exec_ret = exec_and_read(cmd, out);
    if (exec_ret == error::no_error)
    {
        std::regex e(rex);
        std::cmatch cm;

        if (std::regex_search(out.c_str(), cm, e, std::regex_constants::match_any))
        {
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
    log::println("    config        configure libraries and files for a project");
    log::println("    package       package includes and libraries and files for a project");
    log::println("    run           run an executable in a project");
    log::println("    clean         clean a project");
    log::println("    test          run tests for a project");
    log::println("    version       displays ltd version");
    log::println("    help          displays ltd help");
    log::println("");
    log::println("Options:");
}

void print_version()
{
    log::println("ltd version %d.%d", LTD_VERSION_MAJOR, LTD_VERSION_MINOR);
}