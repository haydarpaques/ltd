#ifndef _LTD_INCLUDE_LTD_H_
#define _LTD_INCLUDE_LTD_H_

/**
 * @brief 
 * Contains **{Ltd.}** C++ library.
 * 
 * @details
 * `ltd` namespace is the namespace containing all classes, functions, types 
 * and others belongs to ltd library. The namespace depends on `std` namespace
 * from standard template library.
 *          
 * To use `ltd` namespace, use the namespace by calling keyword `using`.
 * ```C++
 *      using namespace ltd;
 * ``` 
 */ 
namespace ltd {}

#include "cli_args.h"
#include "errors.h"
#include "log.h"
#include "stdalias.h"
#include "test_unit.h"

#endif // _LTD_INCLUDE_LTD_H_