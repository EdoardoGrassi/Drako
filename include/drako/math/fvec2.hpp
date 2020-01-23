#pragma once
#ifndef DRAKO_FVEC2_HPP
#define DRAKO_FVEC2_HPP

/// @file
///
/// @brief  Vector class with two 32bit float components and utility functions.
/// @author Grassi Edoardo
/// @date   Last update: 06-09-2019

#include <array>
#include <cmath>
#include <iostream>
#include <type_traits>

#include "drako/core/preprocessor/compiler_macros.hpp"

namespace drako
{
    /// @brief Packed vector with two single-precision components.
    class alignas(sizeof(float) * 2) vec2 final
    {
    public:
        DRAKO_FORCE_INLINE constexpr explicit vec2() noexcept;
        DRAKO_FORCE_INLINE constexpr explicit vec2(float) noexcept;
        DRAKO_FORCE_INLINE constexpr explicit vec2(float, float) noexcept;
        DRAKO_FORCE_INLINE constexpr explicit vec2(std::array<float, 2>) noexcept;

        DRAKO_FORCE_INLINE constexpr vec2(const vec2&) noexcept = default;
        DRAKO_FORCE_INLINE constexpr vec2& operator=(const vec2&) noexcept = default;

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2& operator+=(float) noexcept;
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2& operator+=(const vec2&) noexcept;

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2& operator-=(float) noexcept;
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2& operator-=(const vec2&) noexcept;

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2& operator*=(float) noexcept;
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2& operator*=(const vec2&) noexcept;

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2& operator/=(float) noexcept;
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2& operator/=(const vec2&) noexcept;

        DRAKO_FORCE_INLINE constexpr friend vec2 operator+(const vec2&, float) noexcept;
        DRAKO_FORCE_INLINE constexpr friend vec2 operator+(float, const vec2&) noexcept;
        DRAKO_FORCE_INLINE constexpr friend vec2 operator+(const vec2&, const vec2&) noexcept;

        DRAKO_FORCE_INLINE constexpr friend vec2 operator-(const vec2&, float) noexcept;
        DRAKO_FORCE_INLINE constexpr friend vec2 operator-(float, const vec2&) noexcept;
        DRAKO_FORCE_INLINE constexpr friend vec2 operator-(const vec2&, const vec2&) noexcept;

        DRAKO_FORCE_INLINE constexpr friend vec2 operator*(const vec2&, float)noexcept;
        DRAKO_FORCE_INLINE constexpr friend vec2 operator*(float, const vec2&)noexcept;
        DRAKO_FORCE_INLINE constexpr friend vec2 operator*(const vec2&, const vec2&)noexcept;

        DRAKO_FORCE_INLINE constexpr friend vec2 operator/(const vec2&, float) noexcept;
        DRAKO_FORCE_INLINE constexpr friend vec2 operator/(float, const vec2&) noexcept;
        DRAKO_FORCE_INLINE constexpr friend vec2 operator/(const vec2&, const vec2&) noexcept;

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float& operator[](size_t) noexcept;
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float  operator[](size_t) const noexcept;

        DRAKO_FORCE_INLINE constexpr friend bool operator==(const vec2&, const vec2&) noexcept;
        DRAKO_FORCE_INLINE constexpr friend bool operator!=(const vec2&, const vec2&) noexcept;

        /// @brief Computes lenght of the vector.
        friend float norm(const vec2&) noexcept;

        constexpr friend float dot(const vec2&, const vec2&) noexcept;

        constexpr friend vec2 abs(const vec2&) noexcept;

        constexpr friend vec2 max(const vec2&, const vec2&) noexcept;

        constexpr friend vec2 min(const vec2&, const vec2&) noexcept;

    private:
#if !defined(DRAKO_API_SIMD)
        float _xy[2];
#else
        __m64 _xyzw;
#endif
    };
    static_assert(std::is_standard_layout_v<vec2>);
    static_assert(std::is_trivially_copyable_v<vec2>);
    static_assert(std::is_nothrow_swappable_v<vec2>);
    static_assert(std::is_nothrow_destructible_v<vec2>);


    DRAKO_FORCE_INLINE constexpr vec2::vec2() noexcept
#if !defined(DRAKO_API_SIMD)
        : _xy{0.f, 0.f}
    {
    }
#else
#error Target not suported
#endif

    DRAKO_FORCE_INLINE constexpr vec2::vec2(float xy) noexcept
#if !defined(DRAKO_API_SIMD)
        : _xy{xy, xy}
    {
    }
#else
#error
#endif

    DRAKO_FORCE_INLINE constexpr vec2::vec2(float x, float y) noexcept
#if !defined(DRAKO_API_SIMD)
        : _xy{x, y}
    {
    }
#else
#error
#endif

    DRAKO_FORCE_INLINE constexpr vec2::vec2(const std::array<float, 2> xy) noexcept
#if !defined(DRAKO_API_SIMD)
        : _xy{xy[0], xy[1]}
    {
    }
#else
#error
#endif

    DRAKO_FORCE_INLINE constexpr vec2& vec2::operator+=(float rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xy[0] += rhs;
        _xy[1] += rhs;
#else
#endif
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr vec2& vec2::operator+=(const vec2& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xy[0] += rhs[0];
        _xy[1] += rhs[1];
#else
#endif
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr vec2& vec2::operator-=(float rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xy[0] -= rhs;
        _xy[1] -= rhs;
#else
#endif
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr vec2& vec2::operator-=(const vec2& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xy[0] -= rhs[0];
        _xy[1] -= rhs[1];
#else
#endif
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr vec2& vec2::operator*=(float rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xy[0] *= rhs;
        _xy[1] *= rhs;
#else
#endif
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr vec2& vec2::operator*=(const vec2& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xy[0] *= rhs[0];
        _xy[1] *= rhs[1];
#else
#endif
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr vec2& vec2::operator/=(float rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xy[0] /= rhs;
        _xy[1] /= rhs;
#else
#endif
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr vec2& vec2::operator/=(const vec2& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xy[0] /= rhs[0];
        _xy[1] /= rhs[1];
#else
#endif
        return *this;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2 operator+(const vec2& lhs, const vec2& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec2(lhs[0] + rhs[0], lhs[1] + rhs[1]);
#else
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2 operator-(const vec2& lhs, const vec2& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec2(lhs[0] - rhs[0], lhs[1] - rhs[1]);
#else
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2 operator*(const vec2& lhs, const vec2& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec2(lhs[0] * rhs[0], lhs[1] * rhs[1]);
#else
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2 operator/(const vec2& lhs, const vec2& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec2(lhs[0] / rhs[0], lhs[1] / rhs[1]);
#else
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float& vec2::operator[](size_t idx) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return _xy[idx];
#else
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float vec2::operator[](size_t idx) const noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return _xy[idx];
#else
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr bool operator==(const vec2& lhs, const vec2& rhs) noexcept
    {
        return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]);
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr bool operator!=(const vec2& lhs, const vec2& rhs) noexcept
    {
        return (lhs[0] != rhs[0]) || (lhs[1] != rhs[1]);
    }

    DRAKO_FORCE_INLINE std::ostream& operator<<(std::ostream& os, const vec2& v) noexcept
    {
        return os << v[0] << ' ' << v[1];
    }

    DRAKO_FORCE_INLINE std::istream& operator>>(std::istream& is, vec2& v) noexcept
    {
        is >> v[0] >> v[1];
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float dot(const vec2& lhs, const vec2& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return (lhs[0] * rhs[0]) + (lhs[1] * rhs[1]);
#else
#endif
    }

    // Returns a vector that is rotated clockwise by 90 degrees.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2 perp_cw(const vec2& v) noexcept
    {
        return vec2(v[1], -v[0]);
    }

    // Returns a vector that is rotated counterclockwise by 90 degrees.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2 perp_ccw(const vec2& v) noexcept
    {
        return vec2(-v[1], v[0]);
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE float norm(const vec2& v) noexcept
    {
        return std::sqrtf(dot(v, v));
    }


    // Returns a vector with the absolute value of each component.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2 abs(const vec2& v) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec2(std::fabs(v[0]), std::fabs(v[1]));
#else

#endif
    }

    // Returns a vector with the smaller value of each component.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2 min(const vec2& lhs, const vec2& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec2(std::fmin(lhs[0], rhs[0]), std::fmin(lhs[1], rhs[1]));
#else
#endif
    }

    // Returns a vector with the greater value of each component.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec2 max(const vec2& lhs, const vec2& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec2(std::fmax(lhs[0], rhs[0]), std::fmax(lhs[1], rhs[1]));
#else
#endif
    }

} // namespace drako

#endif // !DRAKO_FVEC2_HPP