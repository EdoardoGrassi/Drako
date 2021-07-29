#pragma once
#ifndef DRAKO_LOCKFREE_MRMW_QUEUE_HPP
#define DRAKO_LOCKFREE_MRMW_QUEUE_HPP

/// @file
/// @brief       Data structure with FIFO paradigm designed for concurrency.
/// @author      Grassi Edoardo
/// @date        Last update: 16-05-2019

#include "drako/concurrency/lockfree_pool_allocator.hpp"

#include <atomic>
#include <cassert>
#include <limits>
#include <optional>
#include <tuple>

namespace drako::lockfree
{
    /// @brief Thread-safe linearizable container with FIFO policy and bounded capacity.
    template <typename T>
    class MR_MW_Queue
    {
        using node_id = std::uint32_t;
        struct stamped_ref final
        {
            std::uint16_t counter;
            std::uint16_t index;
        };

        static constexpr const auto node_id_mask = (std::numeric_limits<node_id>::max() >> 1);

        static_assert(std::atomic<T*>::is_always_lock_free, "Can't guarantee atomicity");

    public:
        //using allocator = lockfree::Pool<T>;

        /// @brief     Constructor.
        /// @param[in] capacity Max number of objects that can be stored inside the queue.
        explicit MR_MW_Queue(const std::size_t capacity)
            : _alloc{ capacity }
        {
            assert(capacity > 0);
        }

        MR_MW_Queue(const MR_MW_Queue&) noexcept = delete;
        MR_MW_Queue& operator=(const MR_MW_Queue&) noexcept = delete;

        MR_MW_Queue(MR_MW_Queue&&) noexcept = delete;
        MR_MW_Queue& operator=(MR_MW_Queue&&) noexcept = delete;


        /// @brief   Number of objects that the queue can hold.
        ///
        [[nodiscard]] constexpr size_t capacity() const noexcept
        {
            return _alloc.capacity();
        }

        /// @brief Adds an object to the tail of the queue.
        ///
        /// @param[in] value     Object to enqueue.
        ///
        /// @return Returns true if the operation succeeded, false otherwise.
        ///
        [[nodiscard]] bool enque(const T& value) noexcept
        {
            _node* new_node = new (_alloc.allocate(sizeof(_node))) _node(value);
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

        /// @brief Removes an object from the head of the queue.
        ///
        /// @param[out] result   Dequeued object.
        ///
        /// @return Returns true if the operation succeeded, false otherwise.
        ///
        [[nodiscard]] bool deque(T& value) noexcept
        {
            _node* const first = _head.load();
            for (;;)
            {
                _node* const next = first->next.load();

                if (next == nullptr) // only sentinel node in the queue
                {
                    return false;
                }
                else
                {
                    if (_head.compare_exchange_strong(first, next))
                    {
                        value = first->value;
                        // TODO: free 'first' node
                        return true;
                    }
                    // else CAS reloads new value of head in first
                }
            }
        }

    private:
        struct _node final
        {
            const T             value; // Object stored.
            std::atomic<_node*> next;  // Pointer to the next node in the queue.

            explicit _node(const T& value)
                : value(value), next{ nullptr } {}
        };

        lockfree::Pool<T>        _alloc;          // lockfree allocator for the queue nodes
        std::atomic<stamped_ref> _head = nullptr; // pointer to first node
        std::atomic<stamped_ref> _tail = nullptr; // pointer to last node
    };

} // namespace drako::lockfree

#endif // !DRAKO_LOCKFREE_MRMW_QUEUE_HPP