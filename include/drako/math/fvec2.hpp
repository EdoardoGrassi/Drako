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

namespace drako
{
    /// @brief Packed vector with two single-precision components.
    class alignas(sizeof(float) * 2) Vec2 final
    {
    public:
        constexpr explicit Vec2() noexcept
            : _xy{ 0.f, 0.f } {}

        constexpr explicit Vec2(float f) noexcept
            : _xy{ f, f } {}

        constexpr explicit Vec2(float x, float y) noexcept
            : _xy{ x, y } {}

        constexpr explicit Vec2(std::array<float, 2> xy) noexcept
            : _xy{ xy[0], xy[0] } {}

        constexpr Vec2(const Vec2&) noexcept = default;
        constexpr Vec2& operator=(const Vec2&) noexcept = default;


        // constexpr Vec2& operator+=(float) noexcept;
        constexpr Vec2& operator+=(const Vec2& other) noexcept
        {
            _xy[0] += other[0];
            _xy[1] += other[1];
            return *this;
        }

        // constexpr Vec2& operator-=(float) noexcept;
        constexpr Vec2& operator-=(const Vec2& other) noexcept
        {
            _xy[0] -= other[0];
            _xy[1] -= other[1];
            return *this;
        }

        //constexpr Vec2& operator*=(float) noexcept;
        constexpr Vec2& operator*=(const Vec2& other) noexcept
        {
            _xy[0] *= other[0];
            _xy[1] *= other[1];
            return *this;
        }

        //constexpr Vec2& operator/=(float) noexcept;
        constexpr Vec2& operator/=(const Vec2& other) noexcept
        {
            _xy[0] /= other[0];
            _xy[1] /= other[1];
            return *this;
        }


        [[nodiscard]] constexpr float& operator[](std::size_t i) noexcept { return _xy[i]; }
        [[nodiscard]] constexpr float  operator[](std::size_t i) const noexcept { return _xy[i]; }


        /// @brief Computes lenght of the vector.
        friend float norm(const Vec2&) noexcept;

    private:
#if !defined(DRAKO_API_SIMD)
        float _xy[2];
#else
        __m64 _xy;
#endif
    };
    static_assert(std::is_standard_layout_v<Vec2>);
    static_assert(std::is_trivially_copyable_v<Vec2>);
    static_assert(std::is_nothrow_swappable_v<Vec2>);
    static_assert(std::is_nothrow_destructible_v<Vec2>);


    [[nodiscard]] inline constexpr Vec2 operator+(const Vec2& lhs, const Vec2& rhs) noexcept
    {
        return Vec2(lhs[0] + rhs[0], lhs[1] + rhs[1]);
    }

    [[nodiscard]] inline constexpr Vec2 operator-(const Vec2& lhs, const Vec2& rhs) noexcept
    {
        return Vec2(lhs[0] - rhs[0], lhs[1] - rhs[1]);
    }

    [[nodiscard]] inline constexpr Vec2 operator*(const Vec2& lhs, const Vec2& rhs) noexcept
    {
        return Vec2(lhs[0] * rhs[0], lhs[1] * rhs[1]);
    }

    [[nodiscard]] inline constexpr Vec2 operator/(const Vec2& lhs, const Vec2& rhs) noexcept
    {
        return Vec2(lhs[0] / rhs[0], lhs[1] / rhs[1]);
    }

    [[nodiscard]] inline constexpr bool operator==(const Vec2& lhs, const Vec2& rhs) noexcept
    {
        return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]);
    }

    [[nodiscard]] inline constexpr bool operator!=(const Vec2& lhs, const Vec2& rhs) noexcept
    {
        return (lhs[0] != rhs[0]) || (lhs[1] != rhs[1]);
    }

    inline std::ostream& operator<<(std::ostream& os, const Vec2& v) noexcept
    {
        return os << '(' << v[0] << ',' << v[1] << ')';
    }

    inline std::istream& operator>>(std::istream& is, Vec2& v) noexcept
    {
        is >> v[0] >> v[1];
    }

    [[nodiscard]] constexpr float dot(const Vec2& lhs, const Vec2& rhs) noexcept
    {
        return (lhs[0] * rhs[0]) + (lhs[1] * rhs[1]);
    }

    // Returns a vector that is rotated clockwise by 90 degrees.
    [[nodiscard]] inline constexpr Vec2 perp_cw(const Vec2& v) noexcept
    {
        return Vec2(v[1], -v[0]);
    }

    // Returns a vector that is rotated counterclockwise by 90 degrees.
    [[nodiscard]] inline constexpr Vec2 perp_ccw(const Vec2& v) noexcept
    {
        return Vec2(-v[1], v[0]);
    }

    [[nodiscard]] inline float norm(const Vec2& v) noexcept
    {
        return std::sqrtf(dot(v, v));
    }

    // Returns a vector with the absolute value of each component.
    [[nodiscard]] inline constexpr Vec2 abs(const Vec2& v) noexcept
    {
        return Vec2(std::fabs(v[0]), std::fabs(v[1]));
    }

    // Returns a vector with the smaller value of each component.
    [[nodiscard]] inline constexpr Vec2 min(const Vec2& lhs, const Vec2& rhs) noexcept
    {
        return Vec2(std::fmin(lhs[0], rhs[0]), std::fmin(lhs[1], rhs[1]));
    }

    // Returns a vector with the greater value of each component.
    [[nodiscard]] inline constexpr Vec2 max(const Vec2& lhs, const Vec2& rhs) noexcept
    {
        return Vec2(std::fmax(lhs[0], rhs[0]), std::fmax(lhs[1], rhs[1]));
    }

} // namespace drako

#endif // !DRAKO_FVEC2_HPP