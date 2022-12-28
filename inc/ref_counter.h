#ifndef _LTD_INCLUDE_REF_COUNTERS_H_
#define _LTD_INCLUDE_REF_COUNTERS_H_

#include <atomic>

#include "errors.h"
#include "stdalias.h"

namespace ltd
{
    // Forward declaration of object
    // template<typename T, typename A>
    // class object;

    /**
     * @brief
     * Reference counter for atomic reference counting.
     * 
     * Reference counters provide reference counting mechanish that can be used
     * for automatic object deconstruction and memory deallocation.
     * 
     * This reference counter provides atomic counter, which means it is thread
     * safe and lock free. It also provides a 32 bit value space that can be
     * used as flags or other small capacity storage.
     * 
     * The reference counter's total size is 64 bit, with 32 bit atomic counter
     * and 32 bit storage.
     * 
     * This template class provides reference counting mechanism for ltd's 
     * ```object``` and ```pointer``` framework. The reference counter supports
     * 2 modes: wrapped pointer mode and block memory pointer mode.
     * 
     * Wrapped pointer mode is used for encapsulating pointers created by external
     * application. It use ```delete``` operator or a custom deleter function to
     * free the pointer. The memory used for the reference counter itself will be
     * freed by using allocator in template parameter A.
     * 
     * In the memory block mode, the memory for reference counter and the memory
     * for the object is allocated in one swoop. Meaning, the memory for both
     * resource is aligned in one block. Hence, the framework will only destruct
     * the object T without freeing the memory at destruction point of the object.
     * And it will free the memory resource together along with the whole memory
     * block.
     * 
     * The reference counter stores 3 states: the counter that track how many references
     * made to this raw pointer and 2 flags depecting whether the pointer is valid
     * and wheteher we are in wrapper mode or in the memory block mode.
     * 
     * The flags are stored in the pointer address itself. The pointer address will
     * always have 4 '0' bits. The 2 least significant bits are used for this flags.
     * 
     * ```class object``` may invalidate this pointer by calling invalidate function.
     * The function is a private function but it is accessible by ```class object```
     * because it is a friend class to ```ref_counted_ptr```.
     * 
     * Once the pointer is invalidated by ```class object```, it is safe to assume
     * that the pointer is no longer valid even though it is not entirely freed and
     * given back to the allocator.
     *
     */
    class ref_counter
    {
        std::atomic_uint32_t counter;
        std::atomic_uint32_t storage;

    public:
        /**
         * @brief
         * Construct a new ref counter object
         * 
         * The only constructor for ref_counter.
         * 
         * @param data The state to store in the `ref_counter`.
         */
        ref_counter(uint32_t data);

        ref_counter() = delete;
        ref_counter(ref_counter& other) = delete;
        ref_counter(const ref_counter& other) = delete;
        ref_counter& operator=(ref_counter other) = delete;

        /**
         * @brief
         * Increment the reference.
         */
        void inc();

        /**
         * @brief
         * Decrement the reference counter.
         * 
         * @return true If the counter reached 0.
         * @return false If the counter is more than 0.
         */
        bool dec();

        /**
         * @brief
         * Get the data from the reference counter.
         * 
         * @return uint32_t The value of data stored in the reference counter.
         */
        inline uint32_t get_data() const;

        /**
         * @brief
         * Set the data in the reference counter.
         * 
         * @param data The data to store.
         */
        inline void set_data(uint32_t data);

        /**
         * @brief
         * Test bit on the stored data.
         * 
         * @param bit_position The position of the bit to test.
         * @return ret<bool,error> True if it is 1, false if it is 0. error::index_out_of_bound 
         *         if the specified bit is beyond the 31.
         */
        ret<bool,error> test_data_bit(uint8_t bit_position) const;

        /**
         * @brief
         * Set the data bit in the storage to 1.
         * 
         * @param bit_position The bit position to set.
         * @return error error::index_out_of_bound if the specified bit is beyond the 31.
         */
        error set_data_bit(uint8_t bit_position);

        /**
         * @brief
         * Unset the data bit in the storage to 0.
         * 
         * @param bit_position The bit position to unset.
         * @return error error::index_out_of_bound if the specified bit is beyond the 31.
         */
        error unset_data_bit(uint8_t bit_position);
    };
}

#endif //_LTD_INCLUDE_REF_COUNTERS_H_
