#ifndef _LTD_INCLUDE_TEST_UNIT_H_
#define _LTD_INCLUDE_TEST_UNIT_H_

#include <vector>
#include <functional>

namespace ltd
{
    class test_unit
    {
    private:
        std::vector<std::function<void(const test_unit&)>> test_cases;

    public:
        test_unit();

        void test(const std::string& args, std::function<void()> test_function);

        void run(int tcid);

    }; // class test_unit
} // namespace ltd

#endif // _LTD_INCLUDE_TEST_UNIT_H_