/// @file
/// @brief   Thread-safe priority queue templates.
/// @author  Grassi Edoardo
/// @date    Last edited on 15/05/2019

#pragma once
#ifndef DRAKO_LOCKFREE_PRIORITY_QUEUE
#define DRAKO_LOCKFREE_PRIORITY_QUEUE

#include <atomic>
#include <optional>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/concurrency/lockfree_pool_allocator.hpp"

namespace drako::concurrency::lockfree
{
    template <typename T, typename TPriority>
    class priority_queue final
    {
    public:

        DRAKO_NODISCARD bool push(const T& in, const TPriority p) noexcept
        {
            queue_node* new_node = this->pool_allocator.construct(in, p);
            if (new_node == nullptr)
            {
                return false;
            }
            for (;;)
            {
                auto old_node = this->index[p].load();
                new_node->next = old_node->next.load();
                if (this->index[p].compare_exchange_strong(old_node, new_node))
                {
                    return true;
                }
            }
        }

        DRAKO_NODISCARD bool pop(T& out) noexcept
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
            const T value;
            const TPriority priority;
            std::atomic<queue_node*> next = nullptr;

            explicit queue_node(const T& value, const TPriority priority) noexcept
                : value(value), priority(priority)
            {}
        };

        std::atomic<queue_node*> index[10];
        std::atomic<queue_node*> head_node = nullptr;
        lockfree_pool_allocator pool_allocator;
    };
}

#endif // !DRAKO_LOCKFREE_PRIORITY_QUEUE
