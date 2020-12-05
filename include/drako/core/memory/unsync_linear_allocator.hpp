/// @file
/// @brief  Linear allocator
/// @author Grassi Edoardo
/// @date   November 2019

#pragma once
#ifndef DRAKO_UNSYNC_LINEAR_ALLOCATOR_HPP
#define DRAKO_UNSYNC_LINEAR_ALLOCATOR_HPP

#include "drako/core/compiler.hpp"

#include <cstddef>

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
        [[nodiscard]] DRAKO_ALLOCATOR void* DRAKO_RESTRICT
        allocate(std::size_t bytes) noexcept;


        // \brief       Allocates uninitialized memory.
        //
        // \param[in]   bytes   Byte size of the memory block.
        // \param[in]   align   Requested alignment of the memory block.
        //
        // \returns             Pointer to allocated block.
        // \retval      nullptr Allocation failed.
        //
        [[nodiscard]] DRAKO_ALLOCATOR void* DRAKO_RESTRICT
        allocate(std::size_t bytes, std::size_t align) noexcept;


        // \brief       Frees allocated memory.
        //
        // \param[in]   ptr     Pointer to allocated memory block.
        //
        void deallocate(void* DRAKO_RESTRICT p) noexcept;


        // \brief       Frees all allocated memory.
        //
        void release() noexcept;

    private:
#ifdef DRAKO_API_DEBUG
        static inline constexpr size_t dbg_overflow_guard_size = DRKAPI_L1_CACHE_SIZE;

        static inline constexpr uint32_t dbg_allocator_signature = 0xCAFEBABE;
#endif

        std::byte* _base; // Pointer to the first byte of the allocator memory block
        std::byte* _end;  // Pointer to the last byte of the allocator memory block
        std::byte* _curr; // Pointer to the first byte of unallocated memory
    };

} // namespace drako

#endif // !DRAKO_UNSYNC_LINEAR_ALLOCATOR_HPP
