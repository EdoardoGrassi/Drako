#pragma once
#ifndef DRAKO_MATH_UTILITY_HPP
#define DRAKO_MATH_UTILITY_HPP

#include <algorithm>
#include <cmath>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/devel/assertion.hpp"

namespace drako
{
    template <typename T>
    DRAKO_NODISCARD inline constexpr T select(T a, T b, bool condition) noexcept
    {
        static_assert(true == 1);
        static_assert(false == 0);

        int t = static_cast<bool>(condition);
        return a + ((b - a) * t);
    }

    // Clamps a value in range [min, max].
    DRAKO_NODISCARD DRAKO_HPP_NOALIAS DRAKO_FORCE_INLINE constexpr float
    clamp(float t, float minv, float maxv) noexcept
    {
        // DRAKO_PRECON(minv <= maxv);

        t = std::max(t, minv);
        t = std::min(t, maxv);
        return t;
    }

    // Clamps a value in range [+0.0f, +1.0f].
    DRAKO_NODISCARD DRAKO_HPP_NOALIAS DRAKO_FORCE_INLINE constexpr float
    clamp_zero_one(const float t) noexcept
    {
        return std::min(std::max(t, 0.f), 1.f);
    }

    // Find first set (ffs) instruction implemented with compiler intrinsics.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr uint32_t
    builtin_ffs(uint32_t _x) noexcept
    {
#ifdef DRAKO_CC_MSVC

#elif DRAKO_CC_GCC

#endif
        // TODO: impl
        return 0;
    }

    // Find first zero (ffz) instruction implemented with compiler intrinsics.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr uint32_t
    builtin_ffz(uint32_t _x) noexcept
    {
#ifdef DRAKO_CC_MSVC

#elif DRAKO_CC_GCC

#endif
        // TODO: impl
        return 0;
    }

    /// <summary>Computes the next highest power of 2. NOTE: if v is 0 the result is also 0.</summary>
    /// <param name="v">Value to ceil.</param>
    /// <return>Returns the smallest power of 2 equal or greater than value.</return>
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr uint32_t
    ceil_to_pow2(uint32_t v) noexcept
    {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        return v++;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr uint32_t
    floor_to_pow2(uint32_t v) noexcept;

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr uint32_t
    log_pow2(uint32_t v) noexcept
    {
        return builtin_ffs(v);
    }

    /// <summary>Check if a value is an exact power of 2.</summary>
    /// <param name="value">Value to check.</param>
    /// <return>Returns true if the value is a power of 2 (1 included as 2^0), false otherwise.</return>
    DRAKO_NODISCARD DRAKO_HPP_NOALIAS DRAKO_FORCE_INLINE constexpr bool
    is_pow2(size_t v) noexcept
    {
        static_assert(true != 0);
        static_assert(false == 0);

        return v && !(v & (v - 1));
    }

} // namespace drako

#endif // !DRAKO_MATH_UTILITY_HPP