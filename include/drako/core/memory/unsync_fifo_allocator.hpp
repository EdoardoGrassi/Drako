#pragma once
#ifndef DRAKO_UNSYNC_FIFO_ALLOCATOR_HPP
#define DRAKO_UNSYNC_FIFO_ALLOCATOR_HPP

//
// @brief   Memory allocator designed for FIFO sequenced allocations.
// @details Linear allocator implemented with a circular buffer. Not thread safe.
// @author  Grassi Edoardo
//


#include "drako/core/compiler.hpp"
#include "drako/core/memory/memory_core.hpp"

#include <cassert>
#include <cstddef>
#include <memory>

namespace drako
{
    /// @brief Allocator with first-in first-out allocation policy.
    /// @note Not thread safe.
    class UnsyncFifoAlloc
    {
    public:
        explicit UnsyncFifoAlloc(std::size_t capacity)
            : _size{ capacity }
            , _base{ std::make_unique<std::byte[]>(capacity) }
            , _head{ _base.get() }
            , _tail{ _base.get() } {}

        UnsyncFifoAlloc(const UnsyncFifoAlloc&) = delete;
        UnsyncFifoAlloc& operator=(const UnsyncFifoAlloc&) = delete;

        UnsyncFifoAlloc(UnsyncFifoAlloc&&) noexcept;
        UnsyncFifoAlloc& operator=(UnsyncFifoAlloc&&) noexcept;

        [[nodiscard]] std::size_t capacity() const { return _size; }

        [[nodiscard]] DRAKO_ALLOCATOR void* allocate(std::size_t bytes) noexcept;

        [[nodiscard]] DRAKO_ALLOCATOR void* allocate(std::size_t bytes, std::size_t align) noexcept;

        void deallocate(void* DRAKO_RESTRICT p) noexcept;

    private:
        const std::size_t            _size; // Byte size of the allocated buffer.
        std::unique_ptr<std::byte[]> _base; // Pointer to backing memory buffer.
        std::byte*                   _head; // Pointer to the head of the allocation queue
        std::byte*                   _tail; // Pointer to the tail of the allocation queue
    };

} // namespace drako

#endif // !DRAKO_UNSYNC_FIFO_ALLOCATOR_HPP
