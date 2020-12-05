#pragma once
#ifndef DRAKO_MEMORY_UTILITY_HPP
#define DRAKO_MEMORY_UTILITY_HPP

//
//  \brief      Utility functions for memory managment and pointers handling.
//  \author     Grassi Edoardo
//

#include "drako/core/compiler.hpp"
#include "drako/devel/assertion.hpp"

namespace drako
{
    // \brief Byte size of a L1 cache line.
    constexpr const size_t DRKAPI_L1_CACHE_SIZE = 64;

    // \brief Byte pattern for uninitialized memory.
    constexpr const uint32_t DRKAPI_GARBAGE_MEMORY = 0xCCCCCCCC;

    // \brief Byte pattern for memory overflow guards.
    constexpr const uint32_t DRKAPI_OVERFLOW_MEMORY = 0xCCCCCCCC;


    // Pointer to raw memory aligned to a multiple of byte.
    //
    template <size_t Align> //, typename std::enable_if_t<is_valid_alignment(Align), int> = 0>
    class aligned_ptr
    {
    public:
        explicit constexpr aligned_ptr() noexcept = default;

        constexpr aligned_ptr(const aligned_ptr&) noexcept = default;
        constexpr aligned_ptr& operator=(const aligned_ptr&) noexcept = default;

        constexpr operator void*() noexcept { return _p; }

        explicit constexpr aligned_ptr(void* p) noexcept
            : _p(p)
        {
        }

    private:
        std::aligned_storage<1, Align>* _p;
    };

    template <std::size_t Align>
    [[nodiscard]] inline constexpr aligned_ptr<Align> align_cast(void* p) noexcept
    {
        return aligned_ptr<Align>(p);
    }


    // FUNCTION
    //
    // \brief   Fills memory with a garbage value for debugging purposes.
    //
    // \param[in]   ptr     Pointer to the first byte of memory block.
    // \param[in]   size    Byte size of memory block.
    //
    inline void fill_garbage_memory(void* p, std::size_t size) noexcept
    {
        DRAKO_PRECON(p != nullptr);
        DRAKO_PRECON(size > 0);

        std::memset(p, DRKAPI_GARBAGE_MEMORY, size);
    }

    // FUNCTION
    //
    // \brief       Fills memory with a garbage value for debugging purposes.
    //
    // \param[in]   begin   Pointer to the first byte of memory block.
    // \param[in]   end     Pointer to the last byte of memory block.
    //
    inline void fill_garbage_memory(void* _begin, void* _end) noexcept
    {
        DRAKO_PRECON(_begin != nullptr);
        DRAKO_PRECON(_end != nullptr);
        DRAKO_PRECON(std::greater<void*>()(_end, _begin) == true);

        std::memset(_begin, DRKAPI_GARBAGE_MEMORY, static_cast<char*>(_end) - static_cast<char*>(_begin));
    }

    // FUNCTION
    //
    // \brief   Fills guard buffer with a garbage value for debugging purposes.
    //
    // \param[in]   ptr     Pointer to the first byte of memory block.
    // \param[in]   size    Byte size of memory block.
    //
    inline void fill_guard_buffer(void* p, std::size_t size) noexcept
    {
        DRAKO_PRECON(p != nullptr);
        DRAKO_PRECON(size > 0);

        std::memset(p, DRKAPI_OVERFLOW_MEMORY, size);
    }

    // FUNCTION
    //
    // \brief   Fills guard buffer with a garbage value for debugging purposes.
    //
    // \param[in]   begin   Pointer to the first byte of memory block.
    // \param[in]   end     Pointer to the last byte of memory block.
    //
    inline void fill_guard_buffer(void* _begin, void* _end) noexcept
    {
        DRAKO_ASSERT(_begin != nullptr);
        DRAKO_ASSERT(_end != nullptr);
        DRAKO_ASSERT(std::greater<void*>()(_end, _begin) == true);

        std::memset(_begin, DRKAPI_OVERFLOW_MEMORY, static_cast<char*>(_end) - static_cast<char*>(_begin));
    }


    // \brief       Checks if an alignment request is valid.
    //
    // \param[in]   alignment   Requested alignment.
    //
    // \returns     True if the alignment is supported, false otherwise.
    //
    [[nodiscard]] inline constexpr bool is_valid_alignment(std::size_t align) noexcept;


    // \brief       Checks if a pointer is aligned correctly.
    //
    // \param[in]   ptr       Pointer to check (not null).
    // \param[in]   alignment Requested alignment (must be a power of 2).
    //
    // \returns     Returns true if the pointer is aligned correctly, false otherwise.
    //
    [[nodiscard]] inline bool is_aligned(void* p, std::size_t align) noexcept
    {
        DRAKO_ASSERT(p != nullptr, "Pointer can't be null");
        DRAKO_ASSERT(is_valid_alignment(align), "Alignment must be a power of 2");

        return (reinterpret_cast<uintptr_t>(p) & (align - 1)) == 0;
    }


    // FUNCTION
    // Offsets a pointer by a specific amount
    [[nodiscard]] inline constexpr void* offset_pointer(void* p, std::intptr_t offset) noexcept
    {
        // DRAKO_ASSERT(_ptr != nullptr);
        return static_cast<std::byte*>(p) + offset;
    }


    // FUNCTION
    //
    // \brief       Computes offset between a pointer and next aligned pointer.
    //
    // \param[in]   ptr         Base pointer.
    // \param[in]   alignment   Requested alignment.
    //
    // \pre         Argument \p ptr can't be null
    //
    // \returns     Offset to the next correctly aligned pointer.
    // \retval      0       Pointer is already aligned.
    //
    [[nodiscard]] inline ptrdiff_t
    align_up_offset(void* p, std::size_t alignment) noexcept;

    // FUNCTION
    //
    // \brief   Computes offset between a pointer and previous aligned pointer.
    //
    // \param[in]   ptr         Base pointer.
    // \param[in]   alignment   Requested alignment.
    //
    // \pre         Argument \p ptr can't be null
    //
    // \returns     Offset to the next correctly aligned pointer.
    // \retval      0       Pointer is already aligned.
    //
    [[nodiscard]] inline ptrdiff_t
    align_down_offset(void* p, std::size_t align) noexcept;


    template <std::size_t Align>
    [[nodiscard]] inline constexpr aligned_ptr<Align> align_down(void* p) noexcept
    {
        auto       raw_ptr = reinterpret_cast<uintptr_t>(p);
        const auto mask    = Align - 1;

        return raw_ptr & mask;
    }


    template <std::size_t Align>
    [[nodiscard]] inline aligned_ptr<Align> align_up(void* p) noexcept
    {
        const auto raw_ptr = reinterpret_cast<uintptr_t>(p);
        const auto mask    = Align - 1;
        const auto offset  = Align - (raw_ptr & mask);

        return raw_ptr + offset;
    }


    // Rounds down a pointer so its aligned with the specified alignment.
    //
    [[nodiscard]] inline void* align_down(void* p, std::size_t align) noexcept
    {
        DRAKO_ASSERT(p != nullptr, "Pointer can't be null");
        DRAKO_ASSERT(is_valid_alignment(align), "Alignment must be a power of 2");

        const auto raw_ptr = reinterpret_cast<uintptr_t>(p);
        const auto mask    = align - 1;
        const auto aligned = reinterpret_cast<void*>(raw_ptr & mask);

        DRAKO_ASSERT(std::less_equal<void*>()(aligned, p));
        DRAKO_ASSERT(is_aligned(aligned, align));
        return aligned;
    }


    // \brief       Rounds up a pointer so its aligned with the specified alignment.
    //
    // \param[in]   ptr         Pointer to align (must be not null).
    // \param[in]   alignment   Requested alignment (must be a power of 2).
    //
    // \returns     Pointer with the required memory alignment.
    //
    [[nodiscard]] inline void* align_up(void* p, std::size_t align) noexcept
    {
        DRAKO_ASSERT(p != nullptr, "Pointer can't be null");
        DRAKO_ASSERT(is_valid_alignment(align), "Alignment must be a power of 2");

        const auto raw_ptr = reinterpret_cast<uintptr_t>(p);
        const auto mask    = align - 1;
        const auto offset  = align - (raw_ptr & mask);
        const auto aligned = reinterpret_cast<void*>(raw_ptr + offset);

        DRAKO_ASSERT(std::greater_equal<void*>()(aligned, p));
        DRAKO_ASSERT(is_aligned(aligned, align));
        return aligned;
    }


    // FUNCTION
    //
    [[nodiscard]] inline void* align_to_l1_cache(void* p) noexcept
    {
        return align_up(p, DRKAPI_L1_CACHE_SIZE);
    }
} // namespace drako

#endif // !DRAKO_MEMORY_UTILITY_HPP
