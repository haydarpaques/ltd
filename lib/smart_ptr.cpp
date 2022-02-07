#include "../inc/smart_ptr.h"

namespace ltd
{
    bool is_block_smart_ptr(const ref_counter *rc)
    {
        auto [res, err] = rc->test_data_bit(0);
        return res;
    }

    bool is_valid_smart_ptr(const ref_counter *rc)
    {
        auto [res, err] = rc->test_data_bit(1);
        return res;
    }

    void invalidate_smart_ptr(ref_counter *rc)
    {
        rc->unset_data_bit(1);
    }
}