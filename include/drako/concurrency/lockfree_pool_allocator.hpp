/// @file
/// @brief  Threadsafe lockfree implementation of an object pool.
/// @author Grassi Edoardo
/// @date   Last update: 03-09-2019

#pragma once
#ifndef DRAKO_LOCKFREE_POOL_ALLOCATOR_HPP
#    define DRAKO_LOCKFREE_POOL_ALLOCATOR_HPP

#    include <atomic>
#    include <type_traits>

#    include "drako/core/preprocessor/compiler_macros.hpp"
#    include "drako/core/memory/memory_core.hpp"
#    include "drako/math/utility.hpp"
#    include "drako/devel/assertion.hpp"


namespace drako
{
    /// @brief Thread-safe lock-free pool allocator.
    ///
    /// @tparam Size        Byte size of a single memory block.
    /// @tparam Align       Minimum address alignment guaranteed by each memory block.
    ///
    template <size_t Size, size_t Align>
    class lockfree_pool_allocator final
    {
        static_assert(Size > 0, "Block size must be greater than zero.");
        static_assert(is_valid_alignment(Align), "Block alignment must be a power of 2.");
        static_assert(Align <= Size, "Block alignment can't be greater than block size.");

    public:
        explicit lockfree_pool_allocator(size_t count) noexcept;
        ~lockfree_pool_allocator() noexcept;

        lockfree_pool_allocator(const lockfree_pool_allocator&) = delete;
        lockfree_pool_allocator& operator=(const lockfree_pool_allocator&) = delete;

        lockfree_pool_allocator(lockfree_pool_allocator&&) = delete;
        lockfree_pool_allocator& operator=(lockfree_pool_allocator&&) = delete;

        DRAKO_NODISCARD DRAKO_ALLOCATOR void* allocate(size_t bytes) noexcept;

        DRAKO_NODISCARD DRAKO_ALLOCATOR void* allocate(size_t bytes, size_t align) noexcept;

        void deallocate(void* DRAKO_HPP_RESTRICT ptr) noexcept;

        DRAKO_NODISCARD DRAKO_FORCE_INLINE size_t capacity() const noexcept;

        DRAKO_NODISCARD DRAKO_FORCE_INLINE void* raw_memory_buffer() const noexcept { return _pool; }


#    if defined(DRAKO_API_DEBUG)

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

#    endif

    private:
        struct pool_block final
        {
            pool_block* next;
        };

        static_assert(std::atomic<pool_block*>::is_always_lock_free,
            "Required to guarantee lock-free property.");

        std::unique_ptr<pool_block> _pool; // preallocated buffer
        std::atomic<pool_block*> _head;    // head of internal free list

#    if defined(DRKAPI_DEBUG)
        size_t dbg_block_count;
#    endif
    };


    // Thread safe object pool.
    //
    template <typename T>
    using lockfree_object_pool = lockfree_pool_allocator<sizeof(T), alignof(T)>;


#    if defined(DRKAPI_DEBUG)
    static constexpr const size_t dbg_overflow_guard_size = DRKAPI_L1_CACHE_SIZE;
    static constexpr const size_t dbg_underflow_guarg_size = DRKAPI_L1_CACHE_SIZE;
#    endif


    template <size_t Size, size_t Align>
    inline lockfree_pool_allocator<Size, Align>::lockfree_pool_allocator(size_t count) noexcept
    {
        DRAKO_ASSERT(count > 0);

        _pool = static_cast<pool_block*>(std::malloc(
            (Size * count) + sizeof(pool_block)
#    if defined(DRKAPI_DEBUG)
            + dbg_underflow_guarg_size + dbg_overflow_guard_size
#    endif
            ));
        if (_pool == nullptr)
            std::exit(EXIT_FAILURE);

#    if defined(DRKAPI_DEBUG)
        {
            dbg_block_count = count;
            fill_garbage_memory(_pool, Size * count);
        }
#    endif

        _head = _pool;

        // Initialize free list nodes
        auto curr_block = _pool;
        for (auto i = 0; i < (count - 1); ++i)
        {
            curr_block->next = curr_block + Size;
            curr_block = curr_block->next;
        }
        // Set last header as dummy block that always returns nullptr
        curr_block->next = nullptr;
    }

    template <size_t Size, size_t Align>
    inline lockfree_pool_allocator<Size, Align>::~lockfree_pool_allocator() noexcept
    {
        DRAKO_ASSERT(_pool != nullptr);
        std::free(_pool);
    }

    template <size_t Size, size_t Align>
    inline void* lockfree_pool_allocator<Size, Align>::allocate(size_t bytes) noexcept
    {
        DRAKO_ASSERT(bytes > 0, "Zero sized allocations not allowed.");
        DRAKO_ASSERT(bytes <= Size, "Allocation size exceeds pool block size.");

        for (;;)
        {
            auto curr_block = _head.load(std::memory_order_acquire);
            if (curr_block == nullptr)
                return nullptr;

            auto next_block = curr_block->next;

            if (_head.compare_exchange_weak(curr_block, next_block))
            {
#    if defined(DRKAPI_DEBUG)
                {
                    fill_garbage_memory(curr_block, Size);
                }
#    endif
                return curr_block;
            }
        }
    }

    template <size_t Size, size_t Align>
    inline void* lockfree_pool_allocator<Size, Align>::allocate(size_t bytes, size_t alignment) noexcept
    {
        DRAKO_ASSERT(bytes > 0, "Zero sized allocations not allowed.");
        DRAKO_ASSERT(bytes <= Size, "Allocation size exceeds pool block size.");
        DRAKO_ASSERT(is_valid_alignment(alignment), "Allocation alignment must a power of 2.");

        for (;;)
        {
            auto curr = _head.load();
            auto temp_curr = curr->this_block;
            auto temp_next = curr->next;
            if (_head.compare_exchange_strong(curr, temp_next))
            {
#    if defined(DRKAPI_DEBUG)
                {
                    fill_garbage_memory(temp_curr, Size);
                }
#    endif
                return temp_curr;
            }
        }
    }

    template <size_t Size, size_t Align>
    inline void lockfree_pool_allocator<Size, Align>::deallocate(void* DRAKO_HPP_RESTRICT ptr) noexcept
    {
        DRAKO_ASSERT(ptr != nullptr);
        DRAKO_ASSERT(ptr >= _pool);
        DRAKO_ASSERT(ptr <= (_pool + Size * dbg_block_count));
        DRAKO_ASSERT(is_aligned(ptr, Align));

        auto new_head = static_cast<pool_block*>(ptr);
#    if defined(DRKAPI_DEBUG)
        {
            fill_garbage_memory(new_head, Size);
        }
#    endif

        for (;;)
        {
            new_head->next = _head.load();

            if (_head.compare_exchange_strong(new_head->next, new_head))
                return;
        }
    }

} // namespace drako

/*
template <size_t Size, size_t Align>
struct std::allocator_traits<drako::lockfree_pool_allocator<Size, Align>>
{
    using pointer = value_type*;

    static pointer allocate();
};
*/

#endif // !DRAKO_LOCKFREE_POOL_ALLOCATOR_HPP