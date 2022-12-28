#include "errors.h"

namespace ltd
{
    error::error(const char *desc) : code(desc)
    {}

    const char *error::get_description() const
    {
        return code;
    }

    const error error::overflow             ("Overflow");
    const error error::null_pointer         ("Null pointer");
    const error error::index_out_of_bound   ("Index out of bound");
    const error error::invalid_argument     ("Invalid argument");
    const error error::type_conversion      ("Type conversion error");
    const error error::not_found            ("Not found");
    const error error::no_error             ("No error");
    const error error::allocation_failure   ("Allocation failure");
    const error error::deallocation_failure ("Deallocation failure");
    const error error::invalid_address      ("Invalid address");
    const error error::invalid_operation    ("Invalid operation");
    const error error::duplication          ("Duplication");
}
