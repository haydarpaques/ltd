#ifndef _LTD_INCLUDE_STDALIAS_H_
#define _LTD_INCLUDE_STDALIAS_H_

#include <experimental/filesystem>
#include <tuple>

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

} // namespace ltd

#endif // _LTD_INCLUDE_STDALIAS_H_