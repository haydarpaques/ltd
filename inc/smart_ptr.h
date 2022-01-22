#ifndef _LTD_INCLUDE_SMART_PTR_H_
#define _LTD_INCLUDE_SMART_PTR_H_

#include <iostream>
#include <type_traits>
#include <cassert>

#include "memory.h"
#include "ref_counter.h"

namespace ltd
{
    
    bool is_block_smart_ptr(const ref_counter *rc);

    bool is_valid_smart_ptr(const ref_counter *rc);

    void invalidate_smart_ptr(ref_counter *rc);

    /**
     * @brief This the default object deleter for `class object`
     * 
     * Deleters are used for objects and pointers to delete its raw pointers.
     * The default behaviour of this deleter depends on the `block_alloc`  
     * parameter for the `()` operator. If the single_allocation is `false` 
     * then the deleter will treat the pointer as a normal raw pointer that 
     * needs the `delete` operator. If it is `true` then the deleter will 
     * only destruct the object and leave the deallocation to 
     * `object::allocator_type`. 
     * 
     * Implement this struct to have a custom deleter and pass it to the object's
     * template parameter. This would be useful if you have a pointer returned 
     * from external API such as `FILE*` where you can implement the deleter
     * to close the `FILE*` to prefent resource leak.
     * 
     * @tparam T The type of the pointer to delete.
     */
    template<class T>
    struct default_dltr
    {
        void operator ()(T *ptr, bool block_alloc)
        {
            if (block_alloc) {
                memory::destruct(ptr);
            } else  {
                delete ptr;
            }
        }
    };

    template <typename T, typename D, typename A>
    void destroy_smart_ptr(T *ptr, ref_counter *rc)
    {
        D deleter;
        bool block_allocation = is_block_smart_ptr(rc);

        deleter(ptr, block_allocation);

        // Prepare a block struct for memory deallocation
        memory::block blk;

        blk.ptr  = rc;
        blk.size = sizeof(ref_counter);
        
        // If the ref_counter and T was created using block allocation
        // then we set the block size to accomodate both the size of
        // the `ref_counter` and the size of T.
        if (block_allocation)
            blk.size += sizeof(T);

        memory::destruct(rc);

        A allocator;
        allocator.deallocate(blk);
    }

    /**
     * @brief Encapsulates pointers into reference counted pointers.
     * 
     * `class pointer<>` can be obtained from a `class object>`. While the object
     * is still valid, one can create one or more pointers to that particular 
     * object. You can also access the class members by using the `->` operator.
     * 
     * You can also copy the pointer into another pointer and the framework will
     * track of how many reference is made to the object for reference counting.
     * 
     * If the original object is destroyed, the pointer is no longer valid. Hence,
     * it is important to always call `pointer::is_valid()` before accessing the
     * raw pointer's members. 
     * 
     * When a pointer is destroyed, i.e. it goes out of scope, it will release its
     * reference to the original object. The last pointer releasing its reference
     * to the object, if the original `object` is destroyed, will destroy the raw 
     * C++ object, the reference counter object and also deallocate the memory 
     * for the object along with the memory allocated for the reference counter. 
     * 
     * @tparam T The type of the element pointer.
     * @tparam D The type of the deleter.
     * @tparam A The type of the allocator.
     */
    template<typename T, typename D, typename A>
    class pointer
    {
    private:
        T *raw_ptr;
        ref_counter *refcount;

    public: // types
        using element_type   = T;
        using deleter_type   = D;
        using allocator_type = A;

    public: // ctors

        /**
         * @brief Construct a new empty pointer object.
         */
        pointer() : raw_ptr(nullptr), refcount(nullptr) {}

        /**
         * @brief Construct a new pointer from a raw pointer and a referenect counter.
         * 
         * @param ptr        A raw pointer to `class T`.
         * @param refcounter A raw pointer to a reference counter.
         */
        pointer(T *ptr, ref_counter *refcounter)
        {
            if (ptr != nullptr && refcounter != nullptr)
            {
                refcounter->inc();
                refcount = refcounter;
                raw_ptr = ptr;
            }
        }

        /**
         * @brief Construct a new pointer by moving from other pointer.
         * 
         * A move contructor to move the content of another `v` into this
         * new one and empty the other pointer.
         * 
         * @param other The other pointer
         */
        pointer(pointer&& other)
        {
            raw_ptr = other.raw_ptr;
            refcount = other.refcount;

            other.raw_ptr = nullptr;
            other.refcount = nullptr;
        }

        /**
         * @brief Construct a new pointer object from other pointer.
         * 
         * This constructor will copy the other pointer and increase the reference
         * counter by 1 ih the other pointer is a valid pointer.
         * 
         * @param other 
         */
        pointer(const pointer& other) 
        {
            if (other.is_valid())
            {
                other.refcount->inc();

                raw_ptr = other.raw_ptr;
                refcount = other.refcount;
            }
            else
            {
                raw_ptr = nullptr;
                refcount = nullptr;
            }
        }
        
        /**
         * @brief Assignment operator
         * 
         * Copy the pointer from other pointer to this pointer and increase the 
         * reference counter.
         * 
         * @param other 
         * @return pointer& 
         */
        pointer& operator=(const pointer& other)
        {
            if (other.is_valid())
            {
                other.refcount->inc();

                raw_ptr = other.raw_ptr;
                refcount = other.refcount;
            }
            else
            {
                raw_ptr = nullptr;
                refcount = nullptr;
            }
        }

        /**
         * @brief Clears and reset the pointer.
         */
        void clear()
        {
            if (raw_ptr != nullptr)
                raw_ptr = nullptr;

            if (refcount != nullptr)
            {
                if (refcount->dec()) {
                    destroy_smart_ptr<T,D,A>(raw_ptr, refcount);
                }
            }
        }

        /**
         * @brief Test whether the pointer is a valid pointer.
         * 
         * @return true  If the pointer is valid.
         * @return false If the pointer is invalid.
         */
        inline bool is_valid() const
        {
            if (raw_ptr != nullptr && refcount != nullptr && is_valid_smart_ptr(refcount) == true)
                return true;

            clear();
            
            return false;
        }

        ~pointer()
        {
            clear();
        }

        inline T* operator->() { return raw_ptr; }
        inline const T& operator*() const { return *raw_ptr; }
    };

    /**
     * @brief Represents an object on heap and manages its lifetime automatically. 
     * 
     * One of {Ltd.} objective is to provide easier learning curve for beginner
     * programmers to use C++. And one of the challenge is to provide object life-
     * time management that can prevent memory leak without using sophisticated
     * garbage collection mechanism. C++ has several smart pointers while still 
     * allowing raw pointers in its usage. To discourage user evenfurther form 
     * using raw pointers, {Ltd.} provides 'smart pointers' that has a more 
     * familiar feel called `object` and `pointer`.
     * 
     * The usage of `object` and `pointer` is very similar to objects on heap and
     * raw pointers. You create object by using a special command (i.e. new) and
     * you can use pointers to point to that object. When the object is deleted 
     * then the pointers are no longer valid. When an `object` is destroyed then
     * the `pointer`'s pointing to it are no longer valid. The difference is that
     * you can check whether the `pointer` is valid or not without crashing the
     * application. In fact, one should alway call `pointer::is_valid()' before
     * using `pointer`.
     * 
     * `class object<>` and `class pointer<>` are the two major building blocks
     * for automatic object lifetime management in {Ltd.} with each representing
     * objects and pointers respectively. When you create object, you'll get an 
     * instance of `class object<>`. The lifetime of the raw pointer is dictated 
     * by the life of the `class object<>` holding it. 
     * 
     * While the `class object` is still within its lifetime, you can use the 
     * object's pointer by using `->` operator and also you can create a 
     * `class pointer<>` to the `object`. You can call methods and manipulate the 
     * object pointed by 'pointer' as lond ag the `class object` instance of the 
     * object is still valid. 
     * 
     * If an `object` is destroyed, you can no longer create a `pointer` to that
     * `object`. Although the actual object might not be destroyed yet, the 
     * `pointer::is_valid()' will return `false`. This is to prevent access to 
     * invalid raw pointer.
     *  
     * @tparam T 
     * @tparam D 
     * @tparam A 
     */
    template <typename T, 
              typename D=default_dltr<T>, 
              typename A=typename std::conditional<is_defined<memory::global_allocator>, 
                                                              memory::global_allocator, 
                                                              memory::heap_allocator>::type
            >
    class object
    {
    public: // types
        using element_type   = T;
        using deleter_type   = D;
        using allocator_type = A;
        using pointer_type   = pointer<T,D,A>;
        
    private:
        element_type *raw_ptr;
        ref_counter  *refcount;

    public: // ctors

        /**
         * @brief Construct a new empty object
         */
        object() : raw_ptr(nullptr), refcount(nullptr) {}

        /**
         * @brief Construct a new object.
         * 
         * Constructs a new `class object<>` instance by wrapping a raw pointer 
         * of T. This does not create a reference counter, until there is a request 
         * for a pointer. If there's no pointer created on the object's lifetime,  
         * then  there's no reference counter created either and the raw pointer 
         * will be deleted immediately when the object destroyed.
         * 
         * @param ptr A raw pointer to T.
         */
        object(T *ptr) : raw_ptr(ptr), refcount(nullptr) { }

        /**
         * @brief Construct a new object by using a block memory where the 
         * ref_counter and T are allocated in heap in a contiguous manner.
         * This constructor is used by the `make_object<>()` function.
         * 
         * @param ptr 
         * @param rc 
         */
        object(T *ptr, ref_counter *rc) : raw_ptr(ptr), refcount(rc)
        {
            assert(ptr == (T*)(rc+1));
        }

        /**
         * @brief Construct a new object by moving from other object.
         * 
         * A move contructor to move the content of another `object` into this
         * new one and empty the other object.
         * 
         * @param other The other object
         */
        object(object&& other)
        {
            raw_ptr = other.raw_ptr;
            other.raw_ptr = nullptr;

            refcount =other.refcount;
            other.refcount = nullptr;
        }        

        /**
         * @brief Copy constructors are deleted because this class is not copy-able.
         * 
         * @param other 
         */
        object(const object& other) = delete;

        /**
         * @brief Assignment operator is deleted because `object` cannot be copied.
         * 
         * @param other 
         * @return ptr& 
         */
        object& operator=(const object& other) = delete;

    public: // operations
        /**
         * @brief Check if the `object` is null.
         * 
         * An `object` might be null if it has been moved to another object.
         * 
         * @return true  If the object is null.
         * @return false If the onject is not null.
         */
        inline bool is_null() const { return raw_ptr == nullptr; }

        /**
         * @brief Get the pointer object
         * 
         * @return ret<pointer<T,D,A>, error> 
         */
        ret<pointer<T,D,A>, error> get_pointer() 
        {
            if (refcount == nullptr) {
                auto err = make_ref_counter();
                if (err != error::no_error) 
                {
                    // If we failed creating reference counter, return null pointer
                    pointer<T,D,A> ptr;
                    return {ptr, err};
                }
            }

            pointer<T,D,A> ptr = pointer<T,D,A>(raw_ptr, refcount);
            return {ptr, error::no_error};
        }

        /**
         * @brief Checks whether the object is still in a valid state.
         * 
         * @return true If it is valid.
         * @return false If it is not valid.
         */
        bool is_valid() const
        {
            if (raw_ptr == nullptr)
                return false;
            return true;
        }

        inline T* operator->() { return raw_ptr; }
        inline const T& operator*() const { return *raw_ptr; }

        ~object()
        {
            if (raw_ptr != nullptr)
            {
                // If there is a refcount, then handle the refcounter deletion
                // otherwise just delete the raw_ptr.
                if (refcount != nullptr)
                {
                    // If the reference is zero, then destroy the pointer and the refcounter
                    // Otherwise, tell everyone that this pointer is nolonger valid
                    if (refcount->dec())
                    {
                        destroy_smart_ptr<T,D,A>(raw_ptr, refcount);
                    } else {
                        invalidate_smart_ptr(refcount);
                    }

                    refcount = nullptr;
                }
                else
                {
                    deleter_type deleter;
                    deleter(raw_ptr, false);
                }
                raw_ptr = nullptr;
            }
            
        }

    private:
        /**
         * @brief Creates a reference counter.
         * 
         * This function is called when a reference counter needed, i.e when
         * the `get_pointer` function is called and there is no reference counter
         * created yet.
         * 
         * @return error 
         */
        error make_ref_counter()
        {
            if (refcount != nullptr)
                return error::invalid_operation;

            allocator_type allocator;
            auto [blk, err] = allocator.allocate(sizeof(ref_counter));
            
            if (err != error::no_error)
                return err;

            if (blk.ptr == nullptr || blk.size == 0)
                return error::allocation_failure;

            refcount = (ref_counter*) blk.ptr;
            memory::construct(refcount, 1);

            return error::no_error;
        }       
    };

    template<typename T, 
             typename D=default_dltr<T>, 
             typename A=typename std::conditional<is_defined<memory::global_allocator>, 
                                                             memory::global_allocator, 
                                                             memory::heap_allocator>::type,
             typename... P>
    object<T,D,A> make_object(P&&... args)
    {        
        A allocator;

        auto [mem_block, err] = allocator.allocate(sizeof(T)+sizeof(ref_counter));

        if(err != error::no_error)
            return object<T,D,A>(nullptr);

        ref_counter *rc = (ref_counter*)mem_block.ptr;
        T *instance     = (T*)(rc+1);

        memory::construct(instance, std::forward<P>(args)...);
        memory::construct(rc, 1);

        object<T,D,A> obj(instance, rc);
        return obj;
    }
}

#endif // _LTD_INCLUDE_SMART_PTR_H_