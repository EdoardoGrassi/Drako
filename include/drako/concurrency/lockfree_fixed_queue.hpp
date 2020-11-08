/// @file
/// @brief       Data structure with FIFO paradigm designed for concurrency.
/// @author      Grassi Edoardo
/// @date        Last update: 16-05-2019

#pragma once
#ifndef DRAKO_LOCKFREE_FIXED_QUEUE_HPP
#define DRAKO_LOCKFREE_FIXED_QUEUE_HPP

#include "drako/concurrency/lockfree_pool_allocator.hpp"

#include <atomic>
#include <cassert>
#include <limits>
#include <optional>
#include <thread>
#include <tuple>

namespace drako::lockfree
{
    template <typename T, typename Timestamp>
    class atomic_stamped
    {
        static_assert(std::atomic<T>::is_always_lock_free);

    public:
    };


    /// @brief Thread-safe linearizable container with FIFO policy and bounded capacity.
    template <typename T>
    class lockfree_fixed_queue
    {
        using node_id = uint32_t;
        struct stamped_ref final
        {
            std::uint16_t counter;
            std::uint16_t index;
        };

        static constexpr const auto node_id_mask = (std::numeric_limits<node_id>::max() >> 1);

        static_assert(std::atomic<T*>::is_always_lock_free, "Can't guarantee atomicity");

    public:
        using allocator = lockfree_object_pool<T>;

        /// @brief     Constructor.
        /// @param[in] capacity Max number of objects that can be stored inside the queue.
        explicit lockfree_fixed_queue(const std::size_t capacity) noexcept(noexcept(allocator(capacity)))
            : _allocator(capacity)
        {
            assert(capacity > 0);
        }

        lockfree_fixed_queue(const lockfree_fixed_queue&) noexcept = delete;
        lockfree_fixed_queue& operator=(const lockfree_fixed_queue&) noexcept = delete;

        lockfree_fixed_queue(lockfree_fixed_queue&&) noexcept = delete;
        lockfree_fixed_queue& operator=(lockfree_fixed_queue&&) noexcept = delete;


        /// @brief   Number of objects that the queue can hold.
        ///
        [[nodiscard]] constexpr size_t capacity() const noexcept
        {
            return _allocator.capacity();
        }

        /// @brief               Adds an object to the tail of the queue.
        /// @param[in] value     Object to enqueue.
        /// @return              Returns true if the operation succeeded, false otherwise.
        ///
        [[nodiscard]] bool enque(const T& value) noexcept;

        /// @brief               Removes an object from the head of the queue.
        /// @param[out] result   Dequeued object.
        /// @return              Returns true if the operation succeeded, false otherwise.
        ///
        [[nodiscard]] std::tuple<bool, T> deque() noexcept;
        // DRAKO_NODISCARD auto deque(T& result) noexcept;

    private:
        struct node final
        {
            const T            value;          // Object stored.
            std::atomic<node*> next = nullptr; // Pointer to the next node in the queue.

            explicit node(const T& data)
                : value(data) {}
        };

        allocator                _allocator;      // lockfree allocator for the queue nodes
        std::atomic<stamped_ref> _head = nullptr; // pointer to first node
        std::atomic<stamped_ref> _tail = nullptr; // pointer to last node
    };


    template <typename T>
    inline bool lockfree_fixed_queue<T>::enque(const T& value) noexcept
    {
        node* new_node = new (_allocator.allocate(sizeof(node))) node(value);
        if (new_node == nullptr) // pool is out of nodes
            return false;

        for (;;)
        {
            auto last = _tail.load();
            auto next = last->next.load();

            // try append on the tail
            if (last->next.compare_exchange_weak(next, new_node))
            {
                // update tail hint if no other thread already did
                _tail.compare_exchange_strong(last, new_node);
                return true;
            }
        }
    }


    template <typename T>
    inline std::tuple<bool, T> lockfree_fixed_queue<T>::deque() noexcept
    {
        node* const first = _head.load();
        for (;;)
        {
            node* const next = first->next.load();

            if (next == nullptr) // only sentinel node in the queue
            {
                return std::tuple(false, T());
            }
            else
            {
                if (_head.compare_exchange_strong(first, next))
                {
                    auto result = first->value;
                    // TODO: free 'first' node
                    return std::tuple(true, result);
                }
                // else CAS reloads head in first (C++ standard)
            }
        }
    }

} // namespace drako::lockfree

#endif // !DRAKO_LOCKFREE_FIXED_QUEUE_HPP