#ifndef _LTD_INCLUDE_POINTERS_H_
#define _LTD_INCLUDE_POINTERS_H_

#include <atomic>
#include <functional>

#include "memory.h"
#include "ref_counters.h"

namespace ltd
{
    /**
     * Template class pointer provides scoped pointer container.
     * 
     * Use this pointer container to handle non-shared raw pointers to objects.
     * Once this container leaves its scope it will delete the pointer. Unless
     * the content of this container is moved to another container using
     * std::move().
     * 
     * ```C++
     *      ptr<Class> new_container(std::move(old_container));
     * ```
     */
    template<typename T>
    class ptr
    {
        T *raw;

    public:
        ptr() : raw(0) {}

        ptr(T *p) : raw(p) { }

        ptr(ptr&& other)
        {
            raw = other.raw;
            other.raw = nullptr;
        }

        template<typename... A>
        ptr(A&&... args)
        {
            raw = new T(std::forward<A>(args)...);
        }

        ptr(ptr& other) = delete;
        ptr(const ptr& other) = delete;
        ptr& operator=(ptr other) = delete;

        bool is_null() const { return raw == nullptr; }

        inline T* operator->() { return raw; }
        inline const T& operator*() const { return *raw; }

        ~ptr() { if (raw != nullptr) delete raw; }
    };

    template<typename T, typename... A>
    inline ptr<T> make_ptr(A&&... args)
    {
        return ptr<T>(new T(std::forward<A>(args)...));
    }

    /**
     * @brief reference counter
     * 
     * @tparam T
     */
    /*
    template<typename T>
    class ref_counter
    {
        T *raw_ptr;
        std::atomic<int> counter;
        std::function<void(T*)> deleter;

    public:
        ref_counter(T *ptr, std::function<void(T*)> delete_func=nullptr) :
                    raw_ptr(ptr), deleter(delete_func), counter(1) {}

        ref_counter() = delete;
        ref_counter(ref_counter& other) = delete;
        ref_counter(const ref_counter& other) = delete;
        ref_counter& operator=(ref_counter other) = delete;

        ~ref_counter() {
            deleter(raw_ptr);
        }

        template<typename A=memory::heap_allocator>
        static ret<ref_counter*,error> make()
        {
            auto [rc, err] = memory::make<ref_counter, A>();
            if (err != error::no_error)
                return {nullptr, err};

            return {rc, error::no_error};
        }

        static void inc(ref_counter *rc)
        {
            rc->counter++;
        }

        static void dec(ref_counter *rc)
        {
            rc->counter--;
        }
    };
    */

    /**
     * @brief
     * 
     * @tparam T The class pointer type
     * @tparam A The allocator type
     */
    template<typename T, typename A>
    class object
    {
        ref_counted_ptr<T,A> *ptr;

    public:
        object() : raw_ptr(0) {}

        object(T *ptr, std::function<void()> delete_function)
                : raw_ptr(ptr), deleter(delete_function) { }

        object(object&& other)
        {
            ptr = other.ptr;
            other.ptr = nullptr;
        }

        object(object& other) = delete;
        object(const object& other) = delete;
        object& operator=(object other) = delete;

        bool is_null() const { return raw_ptr == nullptr; }

        inline T* operator->() { return raw_ptr; }
        inline const T& operator*() const { return *raw_ptr; }

        ~object() { if (raw_ptr != nullptr) delete raw_ptr; }
    };

    template<typename T, typename A=memory::global_allocator, typename... P>
    inline object<T> make_object(P&&... args)
    {

    }

    template<typename T, typename A=memory::global_allocator, typename... P>
    inline object<T> make_object(P&&... args)
    {
        // Choose allocatot type
        using allocator_type = typename std::conditional<is_defined<A>, A, memory::heap_allocator>::type;
        allocator_type allocator;

        // Allocate 2 objects at once. T and reference counter
        auto [mem_block, err] = allocator.allocate(sizeof(T)+sizeof(ref_counter<T>));

        if (err != error::no_error)
            return object<T>(nullptr);

        T *instance = mem_block.ptr;
        ref_counter<T> *rc = (ref_counter<T>*)(++instance);

        memory::construct(instance, std::forward<P>(args)...);
        memory::construct(rc, instance);

        return object<T>(instance);
    }

    template<typename T>
    inline object<T> attach_object(T *raw_ptr)
    {
        return object<T>(raw_ptr);
    }
} // namespace ltd

#endif // _LTD_INCLUDE_POINTERS_H_
