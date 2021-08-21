#ifndef _LTD_INCLUDE_TEST_UNIT_H_
#define _LTD_INCLUDE_TEST_UNIT_H_

#include <vector>
#include <functional>

namespace ltd
{
    class test_unit
    {
    private:

        std::vector<std::function<void()>> test_cases;

        bool failed;

    public:
        test_unit();
        ~test_unit();

        void test(std::function<void()> test_function);

        void expect(bool condition, const std::string& message);

        void run(int argc, char** argv);

    }; // class test_unit
} // namespace ltd

#endif // _LTD_INCLUDE_TEST_UNIT_H_