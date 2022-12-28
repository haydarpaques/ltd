#ifndef _LTD_INCLUDE_TEST_UNIT_H_
#define _LTD_INCLUDE_TEST_UNIT_H_

#include <vector>
#include <functional>
#include <string>

namespace ltd
{
    /**
     * @brief
     * test_unit provides unit testing framework
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
     */
    class test_unit
    {
    private:
        std::vector<std::function<void()>> test_cases;

        bool failed;

    public:
        /**
         * @brief
         * Construct a new test unit object
         */
        test_unit();

        /**
         * @brief
         * Destroy the test unit object
         */
        ~test_unit();

        /**
         * @brief
         * Register a test case.
         * 
         * @param test_function The test function for the test case.
         */
        void test(std::function<void()> test_function);

        /**
         * @brief
         * Expect a condition to be true for testing purpose.
         * 
         * @details
         * Use this function to test a condition. This will determine whether a test is pass or not.
         * 
         * @param condition
         * @param message
         */
        void expect(bool condition, const std::string& message);

        /**
         * @brief
         * Run the test unit.
         * 
         * @param argc The argc
         * @param argv The argv
         */
        void run(int argc, char** argv);

    }; // class test_unit
} // namespace ltd

#endif // _LTD_INCLUDE_TEST_UNIT_H_
