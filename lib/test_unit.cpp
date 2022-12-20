#include "test_unit.h"
#include "log.h"

namespace ltd
{
    test_unit::test_unit() : failed(false)
    {

    }

    test_unit::~test_unit()
    {
     
    }

    void test_unit::test(std::function<void()> test_function)
    {        
        test_cases.push_back(test_function);
    }

    void test_unit::run(int argc, char** argv)
    {
        if (argc == 1) {
            log::println("%d", test_cases.size());
        } else if (argc == 2) {
            std::string value = argv[1];
            bool isnumber = std::all_of(value.begin(), value.end(), ::isdigit);
            if (isnumber) {
                int index = std::stol(value);

                if (index >= 0 && index < test_cases.size()) {
                    test_cases[index]();
                    if (failed == false) 
                        log::println("-ok-");
                } else {
                    log::println("Invalid test id.");
                    log::println("Specify the test id you want to run or use no argument to get the number of test case.");    
                }
            } else {
                log::println("Invalid use of program arguments.");
                log::println("Specify the test id you want to run or use no argument to get the number of test case.");
            }
        }
    }

    void test_unit::expect(bool condition, const std::string& message)
    {
        if (condition == false) {
            log::println("%s", message);
            failed = true;
        }
    }

} // namespace ltd