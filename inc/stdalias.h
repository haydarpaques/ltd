#ifndef _LTD_INCLUDE_STDALIAS_H_
#define _LTD_INCLUDE_STDALIAS_H_

#include <experimental/filesystem>
#include <tuple>
#include <typeinfo>

namespace ltd
{
    namespace fs = std::experimental::filesystem;

    template<typename... Args>
    using ret = std::tuple<Args...>;

    template<typename... Args>
    auto catch_ret(Args&... args)
    {
        return std::tuple<Args&...>(args...);
    }

    template<typename T, typename = void>
    constexpr bool is_defined = false;

    template<typename T>
    constexpr bool is_defined<T, decltype(typeid(T), void())> = true;
} // namespace ltd

#endif // _LTD_INCLUDE_STDALIAS_H_
