#pragma once
#ifndef DRAKO_BUILTINS_INTRINSICS_HPP
#    define DRAKO_BUILTINS_INTRINSICS_HPP

#    include <cstdint>
#    include <intrin.h>

#    include "drako/core/preprocessor/compiler_macros.hpp"
#    include "drako/core/preprocessor/platform_macros.hpp"

namespace drako
{
    /// @brief Counts bits set to 1.
    DRAKO_NODISCARD constexpr size_t count_ones(uint32_t) noexcept;
    DRAKO_NODISCARD constexpr size_t count_ones(uint64_t) noexcept;

    /// @brief Counts bits set to 0.
    DRAKO_NODISCARD constexpr size_t count_zeros(uint32_t) noexcept;
    DRAKO_NODISCARD constexpr size_t count_zeros(uint64_t) noexcept;

    /// @brief Counts bits before most significand 0.
    DRAKO_NODISCARD constexpr size_t count_leading_ones(uint32_t) noexcept;
    DRAKO_NODISCARD constexpr size_t count_leading_ones(uint64_t) noexcept;

    /// @brief Counts bits before most significand 1.
    DRAKO_NODISCARD constexpr size_t count_leading_zeros(uint32_t) noexcept;
    DRAKO_NODISCARD constexpr size_t count_leading_zeros(uint64_t) noexcept;

    /// @brief Counts bits after less significand 0.
    DRAKO_NODISCARD constexpr size_t count_trailing_ones(uint32_t) noexcept;
    DRAKO_NODISCARD constexpr size_t count_trailing_ones(uint64_t) noexcept;

    /// @brief Counts bits after less significand 1.
    DRAKO_NODISCARD constexpr size_t count_trailing_zeros(uint32_t) noexcept;
    DRAKO_NODISCARD constexpr size_t count_trailing_zeros(uint64_t) noexcept;


    DRAKO_FORCE_INLINE constexpr size_t count_ones(uint32_t _x_) noexcept
    {
#    if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _popcnt32(_x_);
#    else
#        error Target architecture not supported
#    endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_ones(uint64_t _x_) noexcept
    {
#    if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _popcnt64(_x_);
#    else
#        error Target architecture not supported
#    endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_zeros(uint32_t _x_) noexcept
    {
#    if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _popcnt32(~_x_);
#    else
#        error Target architecture not supported
#    endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_zeros(uint64_t _x_) noexcept
    {
#    if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _popcnt64(~_x_);
#    else
#        error Target architecture not supported
#    endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_leading_ones(uint32_t _x_) noexcept
    {
#    if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _lzcnt_u32(~_x_);
#    else
#        error Target architecture not supported
#    endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_leading_ones(uint64_t _x_) noexcept
    {
#    if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _lzcnt_u64(~_x_);
#    else
#        error Target architecture not supported
#    endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_leading_zeros(uint32_t _x_) noexcept
    {
#    if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _lzcnt_u32(_x_);
#    else
#        error Target architecture not supported
#    endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_leading_zeros(uint64_t _x_) noexcept
    {
#    if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _lzcnt_u64(_x_);
#    else
#        error Target architecture not supported
#    endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_trailing_ones(uint32_t _x_) noexcept
    {
#    if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _tzcnt_u32(~_x_);
#    else
#        error Target architecture not supported
#    endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_trailing_ones(uint64_t _x_) noexcept
    {
#    if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _tzcnt_u64(~_x_);
#    else
#        error Target architecture not supported
#    endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_trailing_zeros(uint32_t _x_) noexcept
    {
#    if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _tzcnt_u32(_x_);
#    else
#        error Target architecture not supported
#    endif
    }

    DRAKO_FORCE_INLINE constexpr size_t count_trailing_zeros(uint64_t _x_) noexcept
    {
#    if defined(DRAKO_ARCH_X86) || defined(DRAKO_ARCH_X64)
        return _tzcnt_u64(_x_);
#    else
#        error Target architecture not supported
#    endif
    }
}

#endif // !DRAKO_BUILTINS_INTRINSICS_HPP