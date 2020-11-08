/// @file
/// @brief  Threadsafe lockfree implementation of an object pool.
/// @author Grassi Edoardo
/// @date   Last update: 03-09-2019

#pragma once
#ifndef DRAKO_LOCKFREE_POOL_ALLOCATOR_HPP
#define DRAKO_LOCKFREE_POOL_ALLOCATOR_HPP

#include "drako/core/memory/memory_core.hpp"
#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/math/utility.hpp"

#include <atomic>
#include <bit>
#include <cassert>
#include <type_traits>


namespace drako
{
    /// @brief Thread-safe lock-free pool allocator.
    ///
    /// @tparam Size        Byte size of a single memory block.
    /// @tparam Align       Minimum address alignment guaranteed by each memory block.
    ///
    template <size_t Size, size_t Align>
    //requires std::has_single_bit(Align)
    class lockfree_pool_allocator final
    {
        static_assert(Size > 0, "Block size must be greater than zero.");
        static_assert(is_valid_alignment(Align), "Block alignment must be a power of 2.");
        static_assert(Align <= Size, "Block alignment can't be greater than block size.");

        using _block = std::aligned_storage<Size, Align>;

    public:
        explicit lockfree_pool_allocator(const std::size_t capacity);
        ~lockfree_pool_allocator() noexcept = default;

        lockfree_pool_allocator(const lockfree_pool_allocator&) = delete;
        lockfree_pool_allocator& operator=(const lockfree_pool_allocator&) = delete;

        lockfree_pool_allocator(lockfree_pool_allocator&&) = delete;
        lockfree_pool_allocator& operator=(lockfree_pool_allocator&&) = delete;

        [[nodiscard]] DRAKO_ALLOCATOR void* allocate(size_t bytes) noexcept;
        [[nodiscard]] DRAKO_ALLOCATOR void* allocate(size_t bytes, size_t align) noexcept;

        void deallocate(void* DRAKO_HPP_RESTRICT ptr) noexcept;

        [[nodiscard]] std::size_t capacity() const noexcept;

        [[nodiscard]] void* raw_memory_buffer() const noexcept { return _pool; }


#if defined(DRAKO_API_DEBUG)

        // \brief   Validates an allocated pointer. (debug only)
        //
        // Checks if a pointer could have been allocated from the allocator.
        //
        bool dbg_validate_ptr(void* ptr) const noexcept;

        // \brief   Validates the internal state of the allocator. (debug only)
        //
        // Checks the internal integrity of the allocator.
        //
        bool dbg_validate_allocator() const noexcept;

#endif

    private:
        struct pool_block final
        {
            pool_block* next;
        };

        static_assert(std::atomic<pool_block*>::is_always_lock_free,
            "Required to guarantee lock-free property.");

        std::unique_ptr<_block[]> _pool; // preallocated buffer
        std::atomic<pool_block*>  _head; // head of internal free list

#if defined(DRKAPI_DEBUG)
        size_t dbg_block_count;
#endif
    };


    /// @brief Thread safe object pool.
    template <typename T>
    using lockfree_object_pool = lockfree_pool_allocator<sizeof(T), alignof(T)>;


#if defined(DRKAPI_DEBUG)
    static constexpr const size_t dbg_overflow_guard_size  = DRKAPI_L1_CACHE_SIZE;
    static constexpr const size_t dbg_underflow_guarg_size = DRKAPI_L1_CACHE_SIZE;
#endif


    template <size_t Size, size_t Align>
    inline lockfree_pool_allocator<Size, Align>::lockfree_pool_allocator(size_t count)
    {
        assert(count > 0);

        _pool = std::make_unique<_block[]>(count);

        // Initialize free list nodes
        auto curr_block = _pool;
        for (auto i = 0; i < (count - 1); ++i)
        {
            curr_block->next = curr_block + Size;
            curr_block       = curr_block->next;
        }
        // Set last header as dummy block that always returns nullptr
        curr_block->next = nullptr;
    }

    template <size_t Size, size_t Align>
    inline void* lockfree_pool_allocator<Size, Align>::allocate(size_t bytes) noexcept
    {
        assert(bytes > 0);
        assert(bytes <= Size);

        for (;;)
        {
            auto curr_block = _head.load(std::memory_order_acquire);
            if (curr_block == nullptr)
                return nullptr;

            auto next_block = curr_block->next;

            if (_head.compare_exchange_weak(curr_block, next_block))
            {
#if defined(DRKAPI_DEBUG)
                {
                    fill_garbage_memory(curr_block, Size);
                }
#endif
                return curr_block;
            }
        }
    }

    template <size_t Size, size_t Align>
    inline void* lockfree_pool_allocator<Size, Align>::allocate(size_t bytes, size_t alignment) noexcept
    {
        assert(bytes > 0);
        assert(bytes <= Size);
        assert(std::has_single_bit(alignment));

        for (;;)
        {
            auto curr      = _head.load();
            auto temp_curr = curr->this_block;
            auto temp_next = curr->next;
            if (_head.compare_exchange_strong(curr, temp_next))
            {
#if defined(DRKAPI_DEBUG)
                {
                    fill_garbage_memory(temp_curr, Size);
                }
#endif
                return temp_curr;
            }
        }
    }

    template <size_t Size, size_t Align>
    inline void lockfree_pool_allocator<Size, Align>::deallocate(void* DRAKO_HPP_RESTRICT ptr) noexcept
    {
        assert(ptr != nullptr);
        assert(ptr >= _pool);
        assert(ptr <= (_pool + Size * dbg_block_count));
        assert(is_aligned(ptr, Align));

        auto new_head = static_cast<pool_block*>(ptr);
#if defined(DRKAPI_DEBUG)
        {
            fill_garbage_memory(new_head, Size);
        }
#endif

        for (;;)
        {
            new_head->next = _head.load();

            if (_head.compare_exchange_strong(new_head->next, new_head))
                return;
        }
    }

} // namespace drako

#endif // !DRAKO_LOCKFREE_POOL_ALLOCATOR_HPP