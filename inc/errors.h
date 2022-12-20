#ifndef _LTD_INCLUDE_ERRORS_H_
#define _LTD_INCLUDE_ERRORS_H_

namespace ltd
{
    /**
     * @brief
     * Provides functionalities for handling representation of errors.
     * 
     * @details
     * Class error provide simple representation of error and declaration
     * of some standard error that can be use as comparison.
     * 
     * User error can be defined in adhoc by calling the error's contructor.
     * ```C++
     *      error perform_operation(int value)
     *      {
     *          if (value < 0)
     *              return error("value should not be a negative number");
     *      }
     * ```
     * 
     * Predefined standard errors can also be use to represent the error.
     * ```C++
     *      error perform_operation(int value)
     *      {
     *          if (value < 0)
     *              return error::invalid_argument;
     *      }
     * ```
     * 
     * Equality operator can be used to check the kind of the error.
     * ```C++
     *      auto err = perform_operation(0);
     *      if ( err != no_error)
     *          return err;
     * ```
     */
    class error
    {
        const char *code;

    public:
        error(const char *desc);

        const char *get_description() const;

        friend bool operator == (const error& lhs, const error& rhs);
        friend bool operator != (const error& lhs, const error& rhs);

        /**
         * @brief
         * Represents error related to overflow operation.
         * 
         * @details
         * Use this error for buffer overflow operation or value overflow operation
         * where the value of an operation assigned to a variable that is smaller
         * than the actual value.
         */
        static const error overflow;

        /**
         * @brief
         * Represents error for null pointer exceptions.
         * 
         * @details
         * Use this error for null pointer reference error. If a function if expecting
         * a valid pointer but receiving null, it should return this error. If a function
         * looks up for a value and return pointer to that value, it should not return
         * this error when it fails to fine the value requested. It shuld return
         * error::not_found instead.
         */ 
        static const error null_pointer;

        /**
         * @brief
         * Represents error for index out-of-bound error.
         * 
         * @details
         * Use this error for index out-of-bound errors. Such as when accessing an array
         * the array length or negative value index.
         */
        static const error index_out_of_bound;

        /**
         * @brief
         * Represens error for passing invalid arguments to a function.
         * 
         * @details
         * Use this error for errors regarding unexpected parameters provided. When
         * a function is expecting a certain value but receiving a value that is
         * not as expected, then the function shall return this error.
         */
        static const error invalid_argument;

        /**
         * @brief
         * Represents an error caused by failing type conversion.
         * 
         * @details
         * An operation should return this error when it failed to perform a type
         * conversion.
         */
        static const error type_conversion;

        /**
         * @brief
         * Represetns an error on failing to look for a value.
         * 
         * @details
         * Error returned after an operation of trying to look for a value. This can
         * be a look up, query, or other operation opening a resource.
         */
        static const error not_found;

        /**
         * @brief
         * Repesents a state where there were no error occured.
         * 
         * @details
         * Return this value when the operation is successful and there has not been
         * any kind of failure in the operation.
         */
        static const error no_error;

        /**
         * @brief
         * Error caused by resource allocation failure.
         * 
         * @details
         * Return this value when the operation if failed because of some sort of
         * resource allocation failure i.e. failed memory allocation or no free slot.
         */
        static const error allocation_failure;

        /**
         * @brief
         * Error caused by resource deallocation failure.
         * 
         * @details
         * Return this value when the operation if failed because of some sort of
         * resource deallocation failure i.e. failed memory deallocation.
         */
        static const error deallocation_failure;

        /**
         * @brief
         * Error caused by addressing issues.
         * 
         * @details
         * Return this value when there is an error where the given address is not a valid address.
         */
        static const error invalid_address;

        /**
         * @brief
         * Error caused by some failed operation.
         * 
         * @details
         * Return this value when an operation has failed.
         */
        static const error invalid_operation;

        /**
         * @brief
         * Error caused by duplication occured in the process.
         * 
         * @details
         * Return this value when the operation causes duplication issues or
         * the process cannot be completed because it is resulting in a duplication
         * of data in the system.
         */
        static const error duplication;
    };

    inline bool operator==(const error& lhs, const error& rhs){ return lhs.code == rhs.code; }
    inline bool operator!=(const error& lhs, const error& rhs){ return !(lhs == rhs); }
} // namespace ltd

#endif // _LTD_INCLUDE_ERRORS_H_
