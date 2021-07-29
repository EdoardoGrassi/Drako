#pragma once
#ifndef DRAKO_LOCKFREE_PRIORITY_QUEUE
#define DRAKO_LOCKFREE_PRIORITY_QUEUE

/// @file
/// @brief   Thread-safe priority queue templates.
/// @author  Grassi Edoardo
/// @date    Last edited on 15/05/2019

#include "drako/concurrency/lockfree_pool_allocator.hpp"

#include <atomic>
#include <optional>

namespace drako::lockfree
{
    template <typename T, typename TPriority>
    class PriorityQueue final
    {
    public:
        [[nodiscard]] bool enque(const T& in, const TPriority p) noexcept
        {
            queue_node* new_node = _pool.construct(in, p);
            if (new_node == nullptr)
            {
                return false;
            }
            for (;;)
            {
                auto old_node  = _index[p].load();
                new_node->next = old_node->next.load();
                if (_index[p].compare_exchange_strong(old_node, new_node))
                {
                    return true;
                }
            }
        }

        [[nodiscard]] bool deque(T& out) noexcept
        {
            for (queue_node* old_head;;)
            {
                old_head = head_node.load();
                if (old_head == nullptr)
                {
                    return false;
                }
                else
                {
                    auto old_priority = old_head->priority;
                    index[old_priority].compare_exchange_strong(old_head, nullptr);
                    if (head_node.compare_exchange_strong(old_head, old_head->next))
                    {
                        out = old_head->value;
                        return true;
                    }
                }
            }
        }

    private:
        struct queue_node final
        {
            const T                  value;
            const TPriority          priority;
            std::atomic<queue_node*> next = nullptr;

            explicit queue_node(const T& value, const TPriority priority) noexcept
                : value(value), priority(priority)
            {}
        };

        lockfree::StaticPool<T>  _pool;
        std::atomic<queue_node*> _index[10];
        std::atomic<queue_node*> _head_node = nullptr;
    };
} // namespace drako::lockfree

#endif // !DRAKO_LOCKFREE_PRIORITY_QUEUE
