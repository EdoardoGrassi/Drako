#pragma once
#ifndef DRAKO_UNSYNC_BUDDY_ALLOCATOR_HPP
#define DRAKO_UNSYNC_BUDDY_ALLOCATOR_HPP

#include <cmath>
#include <new>
#include <vector>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/math/utility.hpp"

#include "drako/devel/assertion.hpp"

namespace drako
{
    class unsync_buddy_allocator
    {
    public:

        explicit unsync_buddy_allocator(size_t capacity) noexcept
        {
            DRAKO_ASSERT(capacity > 0);
        }

        explicit unsync_buddy_allocator(size_t min_order, size_t max_order) noexcept
            : _size(block_order_to_size(max_order))
            , _pages(_size)
        {
            DRAKO_ASSERT(min_order > 0);
            DRAKO_ASSERT(max_order > min_order);

            _base = _aligned_malloc(_size, 1 << min_order);
            if (_base == nullptr)
                std::exit(EXIT_FAILURE);

            for (auto b : _pages)
                b = FREE_BLOCK;

            for (auto p : _heads)
                p = nullptr;

            *std::rbegin(_heads) = _base;
        }

        DRAKO_NODISCARD DRAKO_ALLOCATOR
            void* allocate(size_t bytes) noexcept
        {
            DRAKO_ASSERT(bytes > 0);

            if (bytes > _size)
                return nullptr;

            const auto order = block_size_to_order(ceil_to_pow2(bytes));
            
            auto it = std::begin(_heads) + order;
            while (it != std::end(_heads) && *it == nullptr)
                std::next(it);

            if (it != std::end(_heads))
            {
                const auto alloc = it;
                *it = *static_cast<void**>(*it);
                return alloc;
            }
            return nullptr;
        }

        void deallocate(void* DRAKO_HPP_RESTRICT ptr, size_t bytes) noexcept
        {
            DRAKO_ASSERT(ptr != nullptr);
            DRAKO_ASSERT(is_pow2(bytes));

            auto block_order = block_size_to_order(bytes);

            /*for (auto i = block_order; i > 0; --i)
            {
                if (!is_free_block(get_buddy_block(ptr, i), i))
                    break;

                merge_buddy_blocks(ptr, i);
            }*/

            while (block_order > 0 && is_free_block(ptr, block_order))
            {
                // remove from list
                ++block_order;
            }
            
            mark_free_block(ptr, block_order);
        }

    private:

        static constexpr bool FREE_BLOCK = true;
        static constexpr bool USED_BLOCK = false;

        // TODO: vector stores size field which is redundant, may roll custom impl of vector<bool>
        std::vector<bool> _pages;
        void* _base;
        size_t _size;
        void* _heads[10];

        DRAKO_FORCE_INLINE size_t block_size_to_order(size_t bytes) noexcept
        {
            return log_pow2(bytes);
        }

        DRAKO_FORCE_INLINE size_t block_order_to_size(size_t order) noexcept
        {
            return (1 << order);
        }

        DRAKO_FORCE_INLINE void* get_buddy_block(void* ptr, size_t block_order) noexcept
        {
            return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) | static_cast<uintptr_t>(block_order));
        }

        inline void mark_free_block(void* block_start, size_t block_order) noexcept
        {
            for (auto i = 0; i < block_order_to_size(block_order); ++i)
            {
                _pages[i] = FREE_BLOCK;
            }
        }

        // Merges two buddy blocks into a single one.
        inline void merge_buddy_blocks(void* left_block, size_t block_order) noexcept;

        // Splits a block into two buddis.
        inline void split_buddy_blocks(void* block_start, size_t block_order) noexcept;

        // Checks if a block is available.
        inline bool is_free_block(void* block_start, size_t block_order) noexcept
        {
            for (auto i = 0; i < block_order_to_size(block_order); ++i)
            {
                if (_pages[i] != FREE_BLOCK)
                    return false;
            }
            return true;
        }
    };
    static_assert(sizeof(unsync_buddy_allocator) <= std::hardware_constructive_interference_size);
}

#endif // !DRAKO_UNSYNC_BUDDY_ALLOCATOR_HPP
