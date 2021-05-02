#ifndef _LTD_INCLUDE_LTD_H_
#define _LTD_INCLUDE_LTD_H_

/**
 * @brief Contains **{Ltd.}** C++ library.
 * 
 * `ltd` namespace is the namespace containing all classes, functions, types 
 * and others belongs to ltd library. The namespace depends on `std` namespace
 * from standard template library.
 *          
 * To use `ltd` namespace, use the namespace by calling keyword `using`.
 * ```C++
 *      using namespace ltd;
 *      using namespace ltd::memory;
 * ``` 
 */ 
namespace ltd {}

#include "args_opt.h"
#include "errors.h"
#include "fmt.h"
#include "stdalias.h"

#endif // _LTD_INCLUDE_LTD_H_