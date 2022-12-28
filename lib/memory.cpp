#include "memory.h"

namespace ltd
{
    namespace memory
    {
        ret<block,error> null_allocator::allocate(size_t allocation_size)
        {
            return {{nullptr, 0}, error::allocation_failure};
        }

        ret<block,error> null_allocator::allocate_all()
        {
            return {{nullptr, 0}, error::allocation_failure};
        }

        error null_allocator::deallocate(block allocated_block)
        {
            return error::deallocation_failure;
        }

        error null_allocator::deallocate_all()
        {
            return error::deallocation_failure;
        }

        error null_allocator::expand(block& allocated_block, size_t delta)
        {
            return error::allocation_failure;
        }

        ret<bool,error> null_allocator::owns(block mem_block)
        {
            return {false, error::no_error};
        }

        ret<block,error> heap_allocator::allocate(size_t allocation_size)
        {
            block blk;
            blk.ptr  = malloc(allocation_size);
            blk.size = allocation_size;

            if (blk.ptr == nullptr)
                return  {blk, error::allocation_failure};

            return {blk, error::no_error};
        }

        ret<block,error> heap_allocator::allocate_all()
        {
            block blk{nullptr, 0};
            return {blk, error::allocation_failure};
        }

        error heap_allocator::deallocate(block allocated_block)
        {
            if (allocated_block.ptr==nullptr)
                return error::null_pointer;

            free(allocated_block.ptr);

            return error::no_error;
        }

        error heap_allocator::deallocate_all()
        {
            return error::deallocation_failure;
        }

        error heap_allocator::expand(block& allocated_block, size_t delta)
        {
            // TODO: implement this
            return error::allocation_failure;
        }

        ret<bool,error> heap_allocator::owns(block mem_block)
        {
            return {false, error::invalid_operation};
        }
    }
}
