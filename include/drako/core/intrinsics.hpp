#pragma once
#ifndef DRAKO_BUILTINS_INTRINSICS_HPP
#define DRAKO_BUILTINS_INTRINSICS_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"

#if defined(_drako_compiler_msvc)
#include <stdlib.h> // byte swapping intrinsics
#endif

#include <bit>
#include <cstdint>
#include <intrin.h>
#include <type_traits>

namespace drako
{
    /// @brief Swap the order of the bytes.
    [[nodiscard]] inline std::uint16_t byte_swap(std::uint16_t x) noexcept;
    [[nodiscard]] inline std::uint32_t byte_swap(std::uint32_t x) noexcept;
    [[nodiscard]] inline std::uint64_t byte_swap(std::uint64_t x) noexcept;
    [[nodiscard]] inline std::int16_t  byte_swap(std::int16_t x) noexcept;
    [[nodiscard]] inline std::int32_t  byte_swap(std::int32_t x) noexcept;
    [[nodiscard]] inline std::int64_t  byte_swap(std::int64_t x) noexcept;

    template <typename UInt> /* clang-format off */
    requires std::is_integral_v<UInt>&& std::is_unsigned_v<UInt>
    [[nodiscard]] inline constexpr UInt byte_swap(UInt x) noexcept /* clang-format on */
    {
        return byte_swap(x);
    }

    template <typename SInt> /* clang-format off */
    requires std::is_integral_v<SInt> && std::is_signed_v<SInt>
    [[nodiscard]] inline SInt byte_swap(SInt x) noexcept /* clang-format on */
    {
        auto bytes   = std::bit_cast<std::make_unsigned_t<SInt>>(x);
        auto swapped = byte_swap(bytes);
        return std::bit_cast<SInt>(swapped);
    }

    /// @brief Swap the order of the bytes.
    [[nodiscard]] inline std::uint16_t byte_swap(std::uint16_t x) noexcept
    {
#if defined(_drako_compiler_msvc)
        static_assert(sizeof(decltype(x)) == sizeof(unsigned short),
            "MSVC builtin argument doesn't match with std:: integer type.");
        return _byteswap_ushort(x);
#elif defined(_drako_compiler_gcc)
        return __builtin_bswap16(x);
#else
#error Not supported as compiler builtin.
#endif
    }

    /// @brief Swap the order of the bytes.
    [[nodiscard]] inline std::uint32_t byte_swap(std::uint32_t x) noexcept
    {
#if defined(_drako_compiler_msvc)
        static_assert(sizeof(decltype(x)) == sizeof(unsigned long),
            "MSVC builtin argument doesn't match with std:: integer type.");
        return _byteswap_ulong(x);
#elif defined(_drako_compiler_gcc)
        return __builtin_bswap32(x);
#else
#error Not supported as compiler builtin.
#endif
    }

    /// @brief Swap the order of the bytes.
    [[nodiscard]] inline std::uint64_t byte_swap(std::uint64_t x) noexcept
    {
#if defined(_drako_compiler_msvc)
        static_assert(sizeof(decltype(x)) == sizeof(unsigned long long),
            "MSVC builtin argument doesn't match with std:: integer type.");
        return _byteswap_uint64(x);
#elif defined(_drako_compiler_gcc)
        return __builtin_bswap64(x);
#else
#error Not supported as compiler builtin.
#endif
    }

    [[nodiscard]] inline std::int16_t byte_swap(std::int16_t x) noexcept
    {
        auto bytes   = std::bit_cast<std::uint16_t>(x);
        auto swapped = byte_swap(bytes);
        return std::bit_cast<std::int16_t, std::uint16_t>(swapped);
    }


    /// @brief Counts bits set to 1.
    [[deprecated("Use std::popcount()")]]
    [[nodiscard]] constexpr size_t count_ones(uint32_t) noexcept;
    [[deprecated("Use std::popcount()")]]
    [[nodiscard]] constexpr size_t count_ones(uint64_t) noexcept;

    /// @brief Counts bits set to 0.
    [[deprecated("Use std::popcount()")]]
    [[nodiscard]] constexpr size_t count_zeros(uint32_t) noexcept;
    [[deprecated("Use std::popcount()")]]
    [[nodiscard]] constexpr size_t count_zeros(uint64_t) noexcept;

    /// @brief Counts bits before most significand 0.
    [[deprecated("Use std::countl_one()")]]
    [[nodiscard]] constexpr size_t count_leading_ones(uint32_t) noexcept;
    [[deprecated("Use std::countl_one()")]]
    [[nodiscard]] constexpr size_t count_leading_ones(uint64_t) noexcept;

    /// @brief Counts bits before most significand 1.
    [[deprecated("Use std::countl_zero()")]]
    [[nodiscard]] constexpr size_t count_leading_zeros(uint32_t) noexcept;
    [[deprecated("Use std::countl_zero()")]]
    [[nodiscard]] constexpr size_t count_leading_zeros(uint64_t) noexcept;

    /// @brief Counts bits after less significand 0.
    [[deprecated("Use std::countr_one()")]]
    [[nodiscard]] constexpr size_t count_trailing_ones(uint32_t) noexcept;
    [[deprecated("Use std::countr_one()")]]
    [[nodiscard]] constexpr size_t count_trailing_ones(uint64_t) noexcept;

    /// @brief Counts bits after less significand 1.
    [[deprecated("Use std::countr_zero()")]]
    [[nodiscard]] constexpr size_t count_trailing_zeros(uint32_t) noexcept;
    [[deprecated("Use std::countr_zero()")]]
    [[nodiscard]] constexpr size_t count_trailing_zeros(uint64_t) noexcept;


    /*
    DRAKO_FORCE_INLINE constexpr size_t count_ones(uint32_t _x_) noexcept
    {
#if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _popcnt32(_x_);
#else
#error Target architecture not supported
#endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_ones(uint64_t _x_) noexcept
    {
#if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _popcnt64(_x_);
#else
#error Target architecture not supported
#endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_zeros(uint32_t _x_) noexcept
    {
#if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _popcnt32(~_x_);
#else
#error Target architecture not supported
#endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_zeros(uint64_t _x_) noexcept
    {
#if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _popcnt64(~_x_);
#else
#error Target architecture not supported
#endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_leading_ones(uint32_t _x_) noexcept
    {
#if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _lzcnt_u32(~_x_);
#else
#error Target architecture not supported
#endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_leading_ones(uint64_t _x_) noexcept
    {
#if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _lzcnt_u64(~_x_);
#else
#error Target architecture not supported
#endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_leading_zeros(uint32_t _x_) noexcept
    {
#if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _lzcnt_u32(_x_);
#else
#error Target architecture not supported
#endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_leading_zeros(uint64_t _x_) noexcept
    {
#if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _lzcnt_u64(_x_);
#else
#error Target architecture not supported
#endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_trailing_ones(uint32_t _x_) noexcept
    {
#if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _tzcnt_u32(~_x_);
#else
#error Target architecture not supported
#endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_trailing_ones(uint64_t _x_) noexcept
    {
#if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _tzcnt_u64(~_x_);
#else
#error Target architecture not supported
#endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_trailing_zeros(uint32_t _x_) noexcept
    {
#if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _tzcnt_u32(_x_);
#else
#error Target architecture not supported
#endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_trailing_zeros(uint64_t _x_) noexcept
    {
#if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _tzcnt_u64(_x_);
#else
#error Target architecture not supported
#endif
    }
    */
} // namespace drako

#endif // !DRAKO_BUILTINS_INTRINSICS_HPP