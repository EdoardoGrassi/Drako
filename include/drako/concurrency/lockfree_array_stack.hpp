#pragma once
#ifndef DRAKO_LOCKFREE_ARRAY_STACK_HPP
#define DRAKO_LOCKFREE_ARRAY_STACK_HPP

/// @file
/// @author Grassi Edoardo
/// @date   Last updated: 03-09-2019


#include <atomic>

#include "drako/core/compiler.hpp"
#include "drako/concurrency/lockfree_pool_allocator.hpp"

// TODO: end implementation

namespace drako
{
    
    /// @brief Thread-safe container with LIFO policy and fixed maximum capacity.
    ///
    /// @tparam T Type of the objects stored.
    ///
    template<typename T>
    class lockfree_fixed_stack final
    {
    public:

        /// @brief Constructs a stack with the requested capacity.
        ///
        /// @param capacity Number of elements that the stack can hold.
        ///
        /// @remarks Calls std::exit() if memory allocation fails.
        ///
        explicit lockfree_fixed_stack(size_t capacity) noexcept : _allocator(capacity) {}

        /// @brief Inserts an element at the top of the stack.
        ///
        /// The operation fails if the stack is full.
        ///
        /// @param
        /// @return Wheter the operation was succesfull or not.
        ///
        DRAKO_NODISCARD bool push(const T&) noexcept;

        /// @brief Removes an element from the top of the stack.
        ///
        /// The operation fails if the stack is empty.
        ///
        /// @return Wheter the operation was succesfull or not.
        ///
        DRAKO_NODISCARD bool pop() noexcept;

        DRAKO_NODISCARD DRAKO_FORCE_INLINE size_t capacity() const noexcept;

    private:

        struct node
        {
            const T value;
            node*   next;

            explicit node(const T& value) : value(value) {}
        };

        lockfree_object_pool<T> _allocator;
        std::atomic<node*>      _head;
    };


    template<typename T>
    inline bool lockfree_fixed_stack<T>::push(const T& obj) noexcept
    {
        const node* new_head = new (_allocator.allocate(sizeof(T)), std::nothrow) node;
        if (new_head == nullptr)
            return false;

        for (;;)
        {
            new_head->next = _head.load();
            if (_head.compare_exchange_strong(new_head.next, new_head))
                return true;
        }
    }

    template<typename T>
    inline bool lockfree_fixed_stack<T>::pop() noexcept
    {
        return bool();
    }
}

#endif // !DRAKO_LOCKFREE_ARRAY_STACK_HPP
