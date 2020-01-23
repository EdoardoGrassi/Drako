/// @file
///
/// @brief   Vector class with four single-precision components.
/// @details Vector class that leverages support from available SIMD extensions.
/// @author  Grassi Edoardo.
/// @date    Last update: 03-09-2019
///
/// Compilation flags:
/// DRAKO_API_SIMD    Enables hand-written SIMD implementation.

#pragma once
#ifndef DRAKO_VECTOR4_HPP
#define DRAKO_VECTOR4_HPP

#include <array>
#include <cmath>

#if defined(DRAKO_API_SIMD)
#include <intrin.h>
#endif

#include "drako/core/preprocessor/compiler_macros.hpp"

namespace drako
{
    /// @brief Packed vector with four single-precision scalar components.
    class alignas(sizeof(float) * 4) vec4
    {
    public:
        constexpr explicit vec4() noexcept;
        constexpr explicit vec4(float) noexcept;
        constexpr explicit vec4(float, float, float, float) noexcept;
        constexpr explicit vec4(const std::array<float, 4>&) noexcept;

        DRAKO_FORCE_INLINE constexpr vec4(const vec4&) noexcept = default;
        DRAKO_FORCE_INLINE constexpr vec4& operator=(const vec4&) noexcept = default;

        DRAKO_NODISCARD constexpr vec4& operator+=(float) noexcept;
        DRAKO_NODISCARD constexpr vec4& operator+=(const vec4&) noexcept;
        DRAKO_NODISCARD constexpr vec4& operator-=(float) noexcept;
        DRAKO_NODISCARD constexpr vec4& operator-=(const vec4&) noexcept;
        DRAKO_NODISCARD constexpr vec4& operator*=(float) noexcept;
        DRAKO_NODISCARD constexpr vec4& operator*=(const vec4&) noexcept;
        DRAKO_NODISCARD constexpr vec4& operator/=(float) noexcept;
        DRAKO_NODISCARD constexpr vec4& operator/=(const vec4&) noexcept;

        constexpr friend vec4 operator+(const vec4&, float) noexcept;
        constexpr friend vec4 operator+(float, const vec4&) noexcept;
        constexpr friend vec4 operator+(const vec4&, const vec4&) noexcept;

        constexpr friend vec4 operator-(const vec4&, float) noexcept;
        constexpr friend vec4 operator-(float, const vec4&) noexcept;
        constexpr friend vec4 operator-(const vec4&, const vec4&) noexcept;

        constexpr friend vec4 operator*(const vec4&, float)noexcept;
        constexpr friend vec4 operator*(float, const vec4&)noexcept;
        constexpr friend vec4 operator*(const vec4&, const vec4&)noexcept;

        constexpr friend vec4 operator/(const vec4&, float) noexcept;
        constexpr friend vec4 operator/(float, const vec4&) noexcept;
        constexpr friend vec4 operator/(const vec4&, const vec4&) noexcept;

        DRAKO_NODISCARD constexpr float& operator[](size_t) noexcept;
        DRAKO_NODISCARD constexpr float  operator[](size_t) const noexcept;

        constexpr friend bool operator==(const vec4&, const vec4&) noexcept;
        constexpr friend bool operator!=(const vec4&, const vec4&) noexcept;

        friend float norm(const vec4&) noexcept;

        constexpr friend vec4 abs(const vec4&) noexcept;

        constexpr friend float dot(const vec4&, const vec4&) noexcept;

        constexpr friend vec4 max(const vec4&, const vec4&) noexcept;

        constexpr friend vec4 min(const vec4&, const vec4&) noexcept;

    private:
#if !defined(DRAKO_API_SIMD)
        float _xyzw[4];
#else

        __m128 _xyzw;

        DRAKO_FORCE_INLINE constexpr vec4(__m128 data) noexcept
            : _xyzw(data)
        {
        }

        DRAKO_FORCE_INLINE constexpr operator __m128() const noexcept { return _xyzw; }

#endif
    };
    static_assert(std::is_standard_layout_v<vec4>);
    static_assert(std::is_trivially_copyable_v<vec4>);
    static_assert(std::is_nothrow_swappable_v<vec4>);
    static_assert(std::is_nothrow_destructible_v<vec4>);

    // Component-wise multiplication by a scalar.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec4
    operator*(const vec4& v, float f) noexcept
    {
        return v * vec4(f);
    }

    // Component-wise multiplication by a scalar.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec4
    operator*(float f, const vec4& v) noexcept
    {
        return vec4(f) * v;
    }

    // Component-wise division by a scalar.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec4
    operator/(const vec4& v, float f) noexcept
    {
        return v / vec4(f);
    }

    // Component-wise division by a scalar.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec4
    operator/(float f, const vec4& v) noexcept
    {
        return vec4(f) / v;
    }

    inline std::istream& operator>>(std::istream& is, vec4& obj)
    {
        return is >> obj[0] >> obj[1] >> obj[2] >> obj[3];
    }

    inline std::ostream& operator<<(std::ostream& os, const vec4& obj)
    {
        return os << obj[0] << ' ' << obj[1] << ' ' << obj[2] << ' ' << obj[3];
    }

    // Linear interpolation between vectors
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec4 lerp(const vec4& v1, const vec4& v2, float t) noexcept
    {
        return (v2 - v1) * t + v1;
    }

    // Spherical interpolation between vectors
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec4 slerp(const vec4& v1, const vec4& v2, float t) noexcept;

} // namespace drako

#include "drako/math/inl/vector4.inl"

#endif // !DRAKO_VECTOR4_HPP