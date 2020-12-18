#pragma once
#ifndef DRAKO_UNSYNC_LINEAR_ALLOCATOR_HPP
#define DRAKO_UNSYNC_LINEAR_ALLOCATOR_HPP

/// @file
/// @brief  Linear allocator
/// @author Grassi Edoardo
/// @date   November 2019

#include "drako/core/compiler.hpp"

#include <cassert>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace drako
{
    class memory_marker;

    /// @brief Linear monotonic allocator.
    /// @note Not thread safe.
    class UnsyncLinearAllocator
    {
    public:
        class memory_context; // TODO: impl of memory context (ex memory_marker)


        /// @brief Constructs the allocator.
        /// @param[in] bytes Number of bytes of memory reserved for the allocator.
        explicit UnsyncLinearAllocator(std::size_t bytes)
            : _base{ std::make_unique<std::byte[]>(bytes) }
            , _end{ _base.get() + bytes }
            , _curr{ _base.get() }
        {
            assert(bytes > 0);
        }

        UnsyncLinearAllocator(UnsyncLinearAllocator const&) = delete;
        UnsyncLinearAllocator& operator=(UnsyncLinearAllocator const&) = delete;

        UnsyncLinearAllocator(UnsyncLinearAllocator&&) noexcept = default;
        UnsyncLinearAllocator& operator=(UnsyncLinearAllocator&&) noexcept = default;


        /// @brief Allocates uninitialized memory.
        ///
        /// @param[in] bytes Byte size of the memory block.
        ///
        /// @returns Pointer to allocated block.
        /// @retval nullptr Allocation failed.
        ///
        [[nodiscard]] DRAKO_ALLOCATOR void* DRAKO_RESTRICT
        allocate(std::size_t bytes) noexcept;


        /// @brief Allocates uninitialized memory.
        ///
        /// @param[in] bytes Byte size of the memory block.
        /// @param[in] align Requested alignment of the memory block.
        ///
        /// @returns Pointer to allocated block.
        /// @retval nullptr Allocation failed.
        ///
        [[nodiscard]] DRAKO_ALLOCATOR void* DRAKO_RESTRICT
        allocate(std::size_t bytes, std::size_t align) noexcept;


        /// @brief Frees allocated memory.
        ///
        /// @param[in] ptr Pointer to allocated memory block.
        ///
        void deallocate(void* DRAKO_RESTRICT p) noexcept;


        /// @brief Frees all allocated memory.
        void release() noexcept;

    private:
        std::unique_ptr<std::byte[]> _base; // Pointer to the first byte of the allocator memory block
        std::byte*                   _end;  // Pointer to the last byte of the allocator memory block
        std::byte*                   _curr; // Pointer to the first byte of unallocated memory
    };
    static_assert(!std::is_copy_constructible_v<UnsyncLinearAllocator>,
        "Unsyncronized allocator must be a non-copyable type.");
    static_assert(!std::is_copy_assignable_v<UnsyncLinearAllocator>,
        "Unsyncronized allocator must be a non-copyable type.");
    static_assert(std::is_move_constructible_v<UnsyncLinearAllocator>,
        "Unsyncronized allocator must be a movable type.");
    static_assert(std::is_move_assignable_v<UnsyncLinearAllocator>,
        "Unsyncronized allocator must be a movable type.");

} // namespace drako

#endif // !DRAKO_UNSYNC_LINEAR_ALLOCATOR_HPP