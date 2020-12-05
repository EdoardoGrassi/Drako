#pragma once
#ifndef DRAKO_VECTOR4_HPP
#define DRAKO_VECTOR4_HPP

/// @file
///
/// @brief   Vector class with four single-precision components.
/// @details Vector class that leverages support from available SIMD extensions.
/// @author  Grassi Edoardo.
/// @date    Last update: 03-09-2019

#include <array>
#include <cmath>
#include <type_traits>

namespace drako
{
    /// @brief Packed vector with four single-precision scalar components.
    union alignas(sizeof(float) * 4) Vec4
    {
        constexpr explicit Vec4() noexcept
            : x{ 0.f }, y{ 0.f }, z{ 0.f }, w{ 0.f } {}

        constexpr explicit Vec4(float f) noexcept
            : x{ f }, y{ f }, z{ f }, w{ f } {}

        constexpr explicit Vec4(float x, float y, float z, float w) noexcept
            : x{ x }, y{ y }, z{ z }, w{ w } {}

        constexpr explicit Vec4(const float xyzw[4]) noexcept
            : x{ xyzw[0] }, y{ xyzw[1] }, z{ xyzw[2] }, w{ xyzw[3] } {}

        constexpr explicit Vec4(const std::array<float, 4>& xyzw) noexcept
            : x{ xyzw[0] }, y{ xyzw[1] }, z{ xyzw[2] }, w{ xyzw[3] } {}

        constexpr Vec4(const Vec4&) noexcept = default;
        constexpr Vec4& operator=(const Vec4&) noexcept = default;

        constexpr Vec4& operator+=(const Vec4& other) noexcept
        {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }

        constexpr Vec4& operator-=(const Vec4& other) noexcept
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }

        constexpr Vec4& operator*=(const Vec4& other) noexcept
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            w *= other.w;
            return *this;
        }

        constexpr Vec4& operator/=(const Vec4& other) noexcept
        {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            w /= other.w;
            return *this;
        }

        [[nodiscard]] friend constexpr bool operator==(Vec4, Vec4) noexcept = default;
        [[nodiscard]] friend constexpr bool operator!=(Vec4, Vec4) noexcept = default;

        [[nodiscard]] constexpr float& operator[](std::size_t pos) noexcept { return xyzw[pos]; }
        [[nodiscard]] constexpr float  operator[](std::size_t pos) const noexcept { return xyzw[pos]; }

        struct
        {
            float x, y, z, w;
        };
        float xyzw[4];
    };
    static_assert(std::is_standard_layout_v<Vec4>);
    static_assert(std::is_trivially_copyable_v<Vec4>);
    static_assert(std::is_nothrow_swappable_v<Vec4>);
    static_assert(std::is_nothrow_destructible_v<Vec4>);


    [[nodiscard]] inline constexpr Vec4 operator+(Vec4 lhs, Vec4 rhs) noexcept
    {
        return Vec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
    }

    [[nodiscard]] inline constexpr Vec4 operator-(Vec4 lhs, Vec4 rhs) noexcept
    {
        return Vec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
    }

    [[nodiscard]] inline constexpr Vec4 operator*(Vec4 lhs, Vec4 rhs) noexcept
    {
        return Vec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
    }

    [[nodiscard]] inline constexpr Vec4 operator/(Vec4 lhs, Vec4 rhs) noexcept
    {
        return Vec4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
    }

    // Computes the dot product.
    [[nodiscard]] inline constexpr float dot(Vec4 lhs, Vec4 rhs) noexcept
    {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) + (lhs.w * rhs.w);
    }

    // Computes the lenght of the vector.
    [[nodiscard]] inline float norm(Vec4 v) noexcept
    {
        return std::sqrt(dot(v, v));
    }

    // Computes the squared lenght of the vector.
    [[nodiscard]] inline constexpr float sqrd_norm(Vec4 v) noexcept
    {
        return dot(v, v);
    }

    // Linear interpolation between vectors
    [[nodiscard]] inline constexpr Vec4 lerp(Vec4 v1, Vec4 v2, float t) noexcept
    {
        return (v2 - v1) * Vec4{ t } + v1;
    }

    // Spherical interpolation between vectors
    [[nodiscard]] inline constexpr Vec4 slerp(Vec4 v1, Vec4 v2, float t) noexcept;

} // namespace drako

#endif // !DRAKO_VECTOR4_HPP