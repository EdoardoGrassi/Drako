#pragma once
#ifndef DRAKO_MEMORY_UTILITY_HPP
#define DRAKO_MEMORY_UTILITY_HPP

//
//  \brief      Utility functions for memory managment and pointers handling.
//  \author     Grassi Edoardo
//

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/math/utility.hpp"
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
    template <size_t Align>//, typename std::enable_if_t<is_valid_alignment(Align), int> = 0>
    class aligned_ptr
    {
    public:

        explicit constexpr aligned_ptr() noexcept = default;

        constexpr aligned_ptr(const aligned_ptr&) noexcept = default;
        constexpr aligned_ptr& operator=(const aligned_ptr&) noexcept = default;

        DRAKO_FORCE_INLINE constexpr operator void* () noexcept { return _ptr; }

        DRAKO_FORCE_INLINE explicit constexpr aligned_ptr(void* ptr) noexcept
            : _ptr(ptr)
        {
        }

        friend aligned_ptr make_aligned(void* ptr);

    private:

        std::aligned_storage<1, Align>* _ptr;
    };

    template <size_t Align>
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr
        aligned_ptr<Align> make_aligned(void* ptr) noexcept
    {
        return aligned_ptr<Align>(ptr);
    }


    // FUNCTION
    //
    // \brief   Fills memory with a garbage value for debugging purposes.
    //
    // \param[in]   ptr     Pointer to the first byte of memory block.
    // \param[in]   size    Byte size of memory block.
    //
    DRAKO_FORCE_INLINE void fill_garbage_memory(void* _ptr, const size_t _size) noexcept
    {
        DRAKO_PRECON(_ptr != nullptr);
        DRAKO_PRECON(_size > 0);

        std::memset(_ptr, DRKAPI_GARBAGE_MEMORY, _size);
    }

    // FUNCTION
    //
    // \brief       Fills memory with a garbage value for debugging purposes.
    //
    // \param[in]   begin   Pointer to the first byte of memory block.
    // \param[in]   end     Pointer to the last byte of memory block.
    //
    DRAKO_FORCE_INLINE void fill_garbage_memory(void* _begin, void* _end) noexcept
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
    DRAKO_FORCE_INLINE void fill_guard_buffer(void* _ptr, const size_t _size) noexcept
    {
        DRAKO_PRECON(_ptr != nullptr);
        DRAKO_PRECON(_size > 0);

        std::memset(_ptr, DRKAPI_OVERFLOW_MEMORY, _size);
    }

    // FUNCTION
    //
    // \brief   Fills guard buffer with a garbage value for debugging purposes.
    //
    // \param[in]   begin   Pointer to the first byte of memory block.
    // \param[in]   end     Pointer to the last byte of memory block.
    //
    DRAKO_FORCE_INLINE void fill_guard_buffer(void* _begin, void* _end) noexcept
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
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr
        bool is_valid_alignment(size_t align) noexcept
    {
        return is_pow2(align);
    }


    // \brief       Checks if a pointer is aligned correctly.
    //
    // \param[in]   ptr       Pointer to check (not null).
    // \param[in]   alignment Requested alignment (must be a power of 2).
    //
    // \returns     Returns true if the pointer is aligned correctly, false otherwise.
    //
    DRAKO_NODISCARD DRAKO_FORCE_INLINE
        bool is_aligned(void* const ptr, size_t align) noexcept
    {
        DRAKO_ASSERT(ptr != nullptr, "Pointer can't be null");
        DRAKO_ASSERT(is_valid_alignment(align), "Alignment must be a power of 2");

        return (reinterpret_cast<uintptr_t>(ptr) & (align - 1)) == 0;
    }


    // FUNCTION
    // Offsets a pointer by a specific amount
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr
        void* offset_pointer(void* const ptr, intptr_t offset) noexcept
    {
        // DRAKO_ASSERT(_ptr != nullptr);
        return static_cast<std::byte*>(ptr) + offset;
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
    DRAKO_NODISCARD DRAKO_FORCE_INLINE
        ptrdiff_t align_up_offset(void* const ptr, size_t alignment) noexcept
    {
        // DRAKO_ASSERT(ptr != nullptr);
        // DRAKO_ASSERT(is_valid_alignment(alignment));

        // TODO: implementation
        auto raw_ptr = static_cast<std::byte*>(ptr);

        return 0;
    }

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
    DRAKO_NODISCARD DRAKO_FORCE_INLINE
        ptrdiff_t align_down_offset(void* const ptr, size_t align) noexcept
    {
        // TODO: implement
        return 0;
    }


    template<size_t Align>
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr
        aligned_ptr<Align> align_down_ptr(void* const ptr_) noexcept
    {
        auto raw_ptr = reinterpret_cast<uintptr_t>(ptr_);
        const auto mask = Align - 1;

        return raw_ptr & mask;
    }


    template<size_t Align>
    DRAKO_NODISCARD DRAKO_FORCE_INLINE
        aligned_ptr<Align> align_up_ptr(void* const ptr) noexcept
    {
        const auto raw_ptr = reinterpret_cast<uintptr_t>(ptr);
        const auto mask = Align - 1;
        const auto offset = Align - (raw_ptr & mask);

        return raw_ptr + offset;
    }


    // Rounds down a pointer so its aligned with the specified alignment.
    //
    DRAKO_NODISCARD DRAKO_FORCE_INLINE
        void* align_down_pointer(void* const ptr, size_t align) noexcept
    {
        DRAKO_ASSERT(ptr != nullptr, "Pointer can't be null");
        DRAKO_ASSERT(is_valid_alignment(align), "Alignment must be a power of 2");

        const auto raw_ptr = reinterpret_cast<uintptr_t>(ptr);
        const auto mask = align - 1;
        const auto aligned = reinterpret_cast<void*>(raw_ptr & mask);

        DRAKO_ASSERT(std::less_equal<void*>()(aligned, ptr));
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
    DRAKO_NODISCARD DRAKO_FORCE_INLINE
        void* align_up_pointer(void* const ptr, size_t align) noexcept
    {
        DRAKO_ASSERT(ptr != nullptr, "Pointer can't be null");
        DRAKO_ASSERT(is_valid_alignment(align), "Alignment must be a power of 2");

        const auto raw_ptr = reinterpret_cast<uintptr_t>(ptr);
        const auto mask = align - 1;
        const auto offset = align - (raw_ptr & mask);
        const auto aligned = reinterpret_cast<void*>(raw_ptr + offset);

        DRAKO_ASSERT(std::greater_equal<void*>()(aligned, ptr));
        DRAKO_ASSERT(is_aligned(aligned, align));
        return aligned;
    }


    // FUNCTION
    //
    DRAKO_NODISCARD DRAKO_FORCE_INLINE
        void* align_to_l1_cache(void* const ptr) noexcept
    {
        return align_up_pointer(ptr, DRKAPI_L1_CACHE_SIZE);
    }
}

#endif // !DRAKO_MEMORY_UTILITY_HPP
