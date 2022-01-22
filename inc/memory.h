#ifndef _LTD_INCLUDE_MEMORY_H_
#define _LTD_INCLUDE_MEMORY_H_

#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "stdalias.h"

namespace ltd
{
    /**
     * @brief Holds classes and functions
     * 
     * This namespace provides functionalities for memory allocators. The framework
     * partially inspired by Andrei Alexandrescu talks regarding composibles allocators.
     * 
     * The default allocator is assigned to heap_allocator. To override this, 
     * one should declare and implement class global_allocator.
     * 
     * To have a desired bespoke behaviour of an allocator, one can compose two or
     * more allocators. To use it globally, declare it as global_allocator or to
     * use it in a specific class, pass it as a template parameter.
     * 
     * The library uses this framework for ```object::make()``` and internal allocation
     * procedures of containers. 
     */
    namespace memory
    {
        /**
         * Construct object of type T on the given memory address.
         */ 
        template<typename T, typename... Args>
        void construct(T *instance, Args&&... args)
        {
            new (instance) T(std::forward<Args>(args)...);
        }

        /**
         * Destruct the object by calling its destructor. 
         * This does not release the memory.
         */ 
        template<typename T>
        void destruct(T *instance)
        {
            (*instance).~T();
        }

        /**
         * memory::block is used as the container for memory allocation and deallocation.
         */ 
        struct block
        {
            void*  ptr;
            size_t size;
        };

        class global_allocator;

        /**
         * The standard interface for allocators.
         */
        class null_allocator
        {
        public:
            ret<block,error> allocate(size_t allocation_size);
            ret<block,error> allocate_all();

            error deallocate(block allocated_block);
            error deallocate_all();

            error expand(block& allocated_block, size_t delta);

            ret<bool,error> owns(block mem_block);
        };

        class heap_allocator
        {
        public:
            ret<block,error> allocate(size_t allocation_size);
            ret<block,error> allocate_all();

            error deallocate(block allocated_block);
            error deallocate_all();

            error expand(block& allocated_block, size_t delta);

            ret<bool,error> owns(block mem_block);
        };

        template<typename T, 
                 typename A=typename std::conditional<is_defined<memory::global_allocator>, 
                                                                 memory::global_allocator, 
                                                                 memory::heap_allocator>::type, 
                 typename... P>
        ret<T*,error> make(P&&... args)
        {
            using allocator_type = typename std::conditional<is_defined<A>, A, memory::heap_allocator>::type;

            allocator_type allocator;
            auto [b,e] = allocator.allocate(sizeof(T));
            
            if(e != error::no_error)
                return {nullptr, e};

            T *ptr = (T*)b.ptr;
            construct(ptr, std::forward<P>(args)...);
            
            return {ptr, error::no_error};
        }
    }
}

#endif //_LTD_INCLUDE_MEMORY_H_