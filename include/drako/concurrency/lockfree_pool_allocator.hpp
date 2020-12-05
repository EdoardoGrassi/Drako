/// @file
/// @brief  Threadsafe lockfree implementation of an object pool.
/// @author Grassi Edoardo
/// @date   Last update: 03-09-2019

#pragma once
#ifndef DRAKO_LOCKFREE_POOL_ALLOCATOR_HPP
#define DRAKO_LOCKFREE_POOL_ALLOCATOR_HPP

#include "drako/core/compiler.hpp"
#include "drako/core/memory/memory_core.hpp"

#include <atomic>
#include <bit>
#include <cassert>
#include <new>     // std::bad_alloc
#include <numeric> // std::iota
#include <type_traits>

namespace drako::lockfree
{
    /// @brief Thread-safe lock-free pool allocator.
    ///
    /// @tparam Size        Byte size of a single memory block.
    /// @tparam Align       Minimum address alignment guaranteed by each memory block.
    ///
    template <typename T, std::size_t Size> // clang-format off
    requires std::atomic<std::uint32_t>::is_always_lock_free && (Size > 0)
    class StaticPool // clang-format on
    {
    public:
        // types declarations for std::allocator_traits
        using value_type = T;

        explicit StaticPool() noexcept
            : _head{ 0 }
        {
            std::iota(std::begin(_list), std::end(_list), 1);
            std::end(_list)[-1] = empty_pool_value;
        }

        StaticPool(const StaticPool&) = delete;
        StaticPool& operator=(const StaticPool&) = delete;

        StaticPool(StaticPool&&) = delete;
        StaticPool& operator=(StaticPool&&) = delete;

        [[nodiscard]] DRAKO_ALLOCATOR T* DRAKO_RESTRICT allocate(std::size_t n)
        {
            assert(n == 1); // we can only allocate single objects

            for (auto head = _head.load();;)
            {
                const auto block = block_index(head);
                if (block == empty_pool_value) // no free blocks left
                    throw std::bad_alloc{};

                const auto next_tag   = aba_tag(head) + 1;
                const auto next_block = _list[head].load();

                const auto new_head = compose_index_and_tag(next_block, next_tag);
                if (_head.compare_exchange_strong(head, new_head))
                    return reinterpret_cast<T*>(_pool + block);
            }
        }

        void deallocate(T* DRAKO_RESTRICT p, std::size_t n) noexcept
        {
            assert(n == 1); // we can only deallocate single objects
            assert(p);
            assert((p >= _pool) && p < (_pool + Size));

            const auto block = static_cast<std::size_t>(std::distance(_pool, p));
            for (auto head = _head.load();;)
            { // try to push our block as the new head of the free list
                _list[block] = head;
                if (_head.compare_exchange_strong(head, block))
                    return;
                // else the new head value gets loaded by CAS instruction
            }
        }

        [[nodiscard]] constexpr std::size_t capacity() const noexcept { return Size; }

    private:
        using _block = std::aligned_storage_t<sizeof(T), alignof(T)>;

        static_assert(std::atomic<std::uint64_t>::is_always_lock_free,
            "Required to guarantee lock-free property.");

        static const auto empty_pool_value = std::numeric_limits<std::uint32_t>::max();

        std::atomic<std::uint64_t> _head;       // head of free list of unallocated blocks
        std::atomic<std::uint32_t> _list[Size]; // free list blocks
        _block                     _pool[Size]; // memory blocks

        [[nodiscard]] static std::uint32_t block_index(std::uint64_t head) noexcept
        {
            return static_cast<std::uint32_t>(head);
        }

        [[nodiscard]] static std::uint32_t aba_tag(std::uint64_t head) noexcept
        {
            return static_cast<std::uint32_t>(head >> 32);
        }

        [[nodiscard]] static std::uint64_t compose_index_and_tag(std::uint32_t index, std::uint32_t tag)
        {
            return static_cast<std::uint64_t>(index) & (static_cast<std::uint64_t>(tag) << 32);
        }

#if defined(DRKAPI_DEBUG)
        std::atomic<std::size_t> debug_allocated_blocks_count;
#endif
    };

    template <typename T, typename Al = std::allocator<T>>
    class Pool
    {
    public:
        // type declarations for std::allocator_traits
        using value_type = T;

        explicit Pool(std::size_t capacity, Al al = Al());

        [[nodiscard]] constexpr std::size_t capacity() const noexcept;

    private:
        std::unique_ptr<T> _pool;
    };

} // namespace drako::lockfree

#endif // !DRAKO_LOCKFREE_POOL_ALLOCATOR_HPP