#pragma once
#ifndef DRAKO_UNSYNC_FIFO_ALLOCATOR_HPP
#define DRAKO_UNSYNC_FIFO_ALLOCATOR_HPP

//
// @brief   Memory allocator designed for FIFO sequenced allocations.
// @details Linear allocator implemented with a circular buffer. Not thread safe.
// @author  Grassi Edoardo
//

#include <memory>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/memory/memory_core.hpp"

#include "drako/devel/assertion.hpp"
#include "drako/devel/logging.hpp"

namespace drako
{
    // Non-threadsafe allocator with first-in first-out allocation policy.
    //
    class unsync_fifo_allocator
    {
    public:

        // Minimum size (bytes) of a valid preallocated memory block.
        inline static constexpr const size_t min_init_size = 1;

        // Minimum size (bytes) of a valid allocation request.
        inline static constexpr const size_t min_alloc_size = 1;

        explicit unsync_fifo_allocator(size_t bytes) noexcept;
        ~unsync_fifo_allocator() noexcept;

        unsync_fifo_allocator(const unsync_fifo_allocator&) = delete;
        unsync_fifo_allocator& operator=(const unsync_fifo_allocator&) = delete;

        unsync_fifo_allocator(unsync_fifo_allocator&&) noexcept;
        unsync_fifo_allocator& operator=(unsync_fifo_allocator&&) noexcept;


        DRAKO_NODISCARD DRAKO_ALLOCATOR void*
            allocate(size_t bytes) noexcept;

        DRAKO_NODISCARD DRAKO_ALLOCATOR void*
            allocate(size_t bytes, size_t align) noexcept;

        void deallocate(void* DRAKO_HPP_RESTRICT ptr) noexcept;

    private:

        size_t  _size;  // Byte size of the allocated buffer.
        void*   _base;  // Pointer to backing memory buffer.
        void*   _head;  // Pointer to the head of the allocation queue
        void*   _tail;  // Pointer to the tail of the allocation queue
    };


    unsync_fifo_allocator::unsync_fifo_allocator(size_t bytes) noexcept
    {
        DRAKO_ASSERT(bytes > 0);
        _base = std::malloc(bytes);
        if (DRAKO_UNLIKELY(_base == nullptr))
        {
            DRAKO_LOG_FAILURE("Memory acquisition failed.");
            std::exit(EXIT_FAILURE);
        }
    }

    unsync_fifo_allocator::~unsync_fifo_allocator() noexcept
    {
        DRAKO_ASSERT(_base != nullptr);
        std::free(_base);
    }

    DRAKO_NODISCARD DRAKO_ALLOCATOR
        void* unsync_fifo_allocator::allocate(size_t bytes) noexcept
    {
        DRAKO_ASSERT(bytes > 0);

        // TODO: implement
        return nullptr;
    }

    DRAKO_NODISCARD DRAKO_ALLOCATOR
        void* unsync_fifo_allocator::allocate(size_t bytes, size_t align) noexcept
    {
        DRAKO_ASSERT(bytes > 0);
        DRAKO_ASSERT(is_valid_alignment(align));

        // TODO: implement
        return nullptr;
    }

    void unsync_fifo_allocator::deallocate(void* DRAKO_HPP_RESTRICT ptr) noexcept
    {
        DRAKO_ASSERT_NOT_NULL(ptr);
        // TODO: implement
    }
}

#endif // !DRAKO_UNSYNC_FIFO_ALLOCATOR_HPP
