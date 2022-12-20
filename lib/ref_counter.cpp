#include "ref_counter.h"

namespace ltd
{
    ref_counter::ref_counter(uint32_t data) : counter(1), storage(data) {} 
        
    void ref_counter::inc() 
    { 
        ++counter; 
    }

    bool ref_counter::dec() 
    { 
        return 
        --counter == 0; 
    }

    uint32_t ref_counter::get_data() const 
    { 
            return storage.load(); 
    }
        
    void ref_counter::set_data(uint32_t data) 
    { 
        storage.store(data); 
    }
        
    ret<bool,error> ref_counter::test_data_bit(uint8_t bit_position) const
    {
        if (bit_position > 31) 
            return {false, error::index_out_of_bound};

        bool result = (storage.load() & 1 << bit_position) > 0;

        return { result, error::no_error};
    }

    error ref_counter::set_data_bit(uint8_t bit_position)
    {
        if (bit_position > 31) return error::index_out_of_bound;

        storage |= 1 << bit_position;

        return error::no_error;
    }

    error ref_counter::unset_data_bit(uint8_t bit_position)
    {
        if (bit_position > 31) return error::index_out_of_bound;

        storage &= ~(1 << bit_position);

        return error::no_error;
    }
}