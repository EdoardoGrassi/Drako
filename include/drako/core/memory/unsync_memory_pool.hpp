#pragma once
#ifndef DRAKO_UNSYNC_MEMORY_POOL_HPP
#define DRAKO_UNSYNC_MEMORY_POOL_HPP

#include <new>
#include <type_traits>

#include "drako/core/compiler.hpp"
#include "drako/core/memory/memory_core.hpp"

namespace drako
{
    // CLASS TEMPLATE
    // Unsynchronized memory pool.
    template <size_t Size, size_t Align = DRKAPI_L1_CACHE_SIZE>
    class unsync_memory_pool
    {
    public:
        explicit constexpr unsync_memory_pool(size_t capacity) noexcept
        {
            _pool = static_cast<pool_node*>(_aligned_malloc(sizeof(pool_node) * capacity, Align));
            if (_pool == nullptr)
            {
                std::exit(EXIT_FAILURE);
            }
            _head = _pool;
            for (auto i = 0; i < capacity - 1; ++i)
            {
                _pool[i]->next = _pool[i + 1];
            }
            _pool[capacity - 1]->next = nullptr;
        }

        ~unsync_memory_pool() noexcept
        {
            if (_pool != nullptr)
            {
                _aligned_free(_pool);
            }
        }

        unsync_memory_pool(const unsync_memory_pool&) = delete;
        unsync_memory_pool& operator=(const unsync_memory_pool&) = delete;

        unsync_memory_pool(unsync_memory_pool&& other) noexcept;
        unsync_memory_pool& operator=(unsync_memory_pool&& other) noexcept;


        // Acquires bytes from the underlying memory resource.
        // Complexity: O(1)
        //
        DRAKO_NODISCARD DRAKO_ALLOCATOR
            aligned_ptr<Align>
            allocate(size_t bytes) noexcept
        {
            const auto old_head = _head;
            _head               = (_head != nullptr) _head->next : nullptr;
            return old_head;
        }


        // Releases bytes to the underlying memory resource.
        // Complexity: O(1)
        //
        void deallocate(aligned_ptr<Align> DRAKO_HPP_RESTRICT ptr) noexcept
        {
        }

    private:
        union pool_node
        {
            pool_node*                          next;
            std::aligned_storage_t<Size, Align> value;
        };
        static_assert(sizeof(pool_node) == Size, "Pool block violates interface requirements");
        static_assert(alignof(pool_node) == Align, "Pool block violates interface requirements");

        pool_node* _pool;
        pool_node* _head;
    };

} // namespace drako

#endif // !DRAKO_UNSYNC_MEMORY_POOL_HPP
