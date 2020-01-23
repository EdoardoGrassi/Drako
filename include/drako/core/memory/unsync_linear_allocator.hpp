/// @file
/// @brief  Linear allocator
/// @author Grassi Edoardo
/// @date   November 2019

#pragma once
#ifndef DRAKO_UNSYNC_LINEAR_ALLOCATOR_HPP
#define DRAKO_UNSYNC_LINEAR_ALLOCATOR_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/memory/memory_core.hpp"

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

namespace drako
{
    class memory_marker;

    // Thread-unsafe allocator.
    //
    class unsync_linear_allocator
    {
    public:

        class memory_context; // TODO: impl of memory context (ex memory_marker)


        // \brief       Constructor.
        // \param[in]   capacity    Number of bytes of memory reserved for the allocator.
        explicit unsync_linear_allocator(size_t bytes) noexcept;
        ~unsync_linear_allocator() noexcept;

        unsync_linear_allocator(unsync_linear_allocator const&) = delete;
        unsync_linear_allocator& operator=(unsync_linear_allocator const&) = delete;

        unsync_linear_allocator(unsync_linear_allocator&&) noexcept;
        unsync_linear_allocator& operator=(unsync_linear_allocator&&) noexcept;


        // \brief       Allocates uninitialized memory.
        //
        // \param[in]   bytes    Byte size of the memory block.
        //
        // \returns     Pointer to allocated block.
        // \retval      nullptr  Allocation failed.
        //
        DRAKO_NODISCARD DRAKO_ALLOCATOR DRAKO_FORCE_INLINE
            void* allocate(size_t bytes) noexcept;


        // \brief       Allocates uninitialized memory.
        //
        // \param[in]   bytes   Byte size of the memory block.
        // \param[in]   align   Requested alignment of the memory block.
        //
        // \returns             Pointer to allocated block.
        // \retval      nullptr Allocation failed.
        //
        DRAKO_NODISCARD DRAKO_ALLOCATOR inline
            void* allocate(size_t bytes, size_t align) noexcept;


        // \brief       Frees allocated memory.
        //
        // \param[in]   ptr     Pointer to allocated memory block.
        //
        DRAKO_FORCE_INLINE void deallocate(void* DRAKO_HPP_RESTRICT ptr) noexcept;


        // \brief       Frees all allocated memory.
        //
        DRAKO_FORCE_INLINE void release() noexcept;

    private:

        #ifdef DRAKO_API_DEBUG
        static inline constexpr size_t dbg_overflow_guard_size = DRKAPI_L1_CACHE_SIZE;

        static inline constexpr uint32_t dbg_allocator_signature = 0xCAFEBABE;
        #endif

        std::byte* _base;   // Pointer to the first byte of the allocator memory block
        std::byte* _end;    // Pointer to the last byte of the allocator memory block
        std::byte* _curr;   // Pointer to the first byte of unallocated memory
    };


    unsync_linear_allocator::unsync_linear_allocator(size_t bytes) noexcept
    {
        DRAKO_ASSERT(bytes > 0);

        #ifdef DRAKO_API_DEBUG
        {
            // Add space for underflow and overflow buffers
            bytes += 2 * dbg_overflow_guard_size;
        }
        #endif

        _base = static_cast<std::byte*>(std::malloc(bytes));
        if (DRAKO_UNLIKELY(nullptr == _base))
        {
            DRAKO_LOG_FAILURE("Memory acquisition failed");
            std::exit(EXIT_FAILURE);
        }

        #ifdef DRAKO_API_DEBUG
        {
            // Add offset for underflow buffer
            _base += dbg_overflow_guard_size;
        }
        #endif

        _end = _base + bytes - 1;
        _curr = _base;

        #ifdef DRAKO_API_DEBUG
        {
            // Initialize memory with debug patterns
            fill_guard_buffer(_base, dbg_overflow_guard_size);
            fill_garbage_memory(_base, _end);
            fill_guard_buffer(_end + 1, dbg_overflow_guard_size);
        }
        #endif
    }

    unsync_linear_allocator::~unsync_linear_allocator() noexcept
    {
        #ifdef DRAKO_API_DEBUG
        {
            _curr -= dbg_overflow_guard_size;
        }
        #endif
        DRAKO_ASSERT(_curr == _base, "Allocator destroyed while some memory is still allocated");
        if (_base != nullptr)
            std::free(_base);
    }

    unsync_linear_allocator::unsync_linear_allocator(unsync_linear_allocator&& other) noexcept
        : _base(other._base)
        , _end(other._end)
        , _curr(other._curr)
    {
        other._base = nullptr;
    }

    unsync_linear_allocator& unsync_linear_allocator::operator=(unsync_linear_allocator&& other) noexcept
    {
        if (this != &other)
        {
            _base = other._base;
            _end = other._base;
            _curr = other._curr;
            other._base = nullptr;
        }
        return *this;
    }

    void* unsync_linear_allocator::allocate(size_t bytes) noexcept
    {
        DRAKO_ASSERT(bytes > 0);

        auto new_curr = _curr + bytes;
        #ifdef DRAKO_API_DEBUG
        {
            new_curr += sizeof(dbg_allocator_signature);
        }
        #endif

        if (new_curr > _end)  // not enough memory available
            return nullptr;

        auto ptr = _curr;
        #ifdef DRAKO_API_DEBUG
        {
            *ptr = dbg_allocator_signature;
            ptr += sizeof(dbg_allocator_signature);
        }
        #endif
        _curr = new_curr;
        return ptr;
    }

    void* unsync_linear_allocator::allocate(size_t bytes, size_t align) noexcept
    {
        DRAKO_ASSERT(bytes > 0);
        DRAKO_ASSERT(is_valid_alignment(align));

        auto new_curr = static_cast<std::byte*>(align_up_pointer(_curr, align)) + bytes;
        #ifdef DRAKO_API_DEBUG
        {
            new_curr += sizeof(dbg_allocator_signature);
        }
        #endif

        if (new_curr > _end) // not enough memory available
            return nullptr;

        auto ptr = _curr;
        _curr = new_curr;
        return ptr;
    }

    void unsync_linear_allocator::deallocate(void* DRAKO_HPP_RESTRICT ptr) noexcept
    {
        DRAKO_ASSERT(ptr != nullptr);
        DRAKO_ASSERT(ptr < _curr);
        DRAKO_ASSERT(ptr >= _base);

        #ifdef DRAKO_API_DEBUG
        {
            ptr = static_cast<std::byte*>(ptr) - sizeof(dbg_allocator_signature);
            debug::assert(*static_cast<uint32_t*>(ptr) == dbg_allocator_signature);
        }
        #endif
        _curr = static_cast<std::byte*>(ptr);
    }

    void unsync_linear_allocator::release() noexcept
    {
        _curr = _base;
        #ifdef DRAKO_API_DEBUG
        {
            fill_garbage_memory(_base, _mem_end);
        }
        #endif
    }
}

#endif // !DRAKO_UNSYNC_LINEAR_ALLOCATOR_HPP
