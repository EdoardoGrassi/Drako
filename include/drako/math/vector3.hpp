#pragma once
#ifndef DRAKO_VECTOR3_HPP
#define DRAKO_VECTOR3_HPP

/// @file
///
/// @author Grassi Edoardo
/// @date   Last update 04-09-2019

#include <array>
#include <cmath>
#include <type_traits>

namespace drako
{
    class norm3;

    /// @brief Packed vector with three single-precision scalar components.
    union alignas(sizeof(float) * 4) Vec3
    {
        constexpr explicit Vec3() noexcept
            : x{ 0.f }, y{ 0.f }, z{ 0.f } {}

        constexpr explicit Vec3(float f) noexcept
            : x{ f }, y{ f }, z{ f } {}

        constexpr explicit Vec3(float x, float y, float z) noexcept
            : x{ x }, y{ y }, z{ z } {}

        constexpr explicit Vec3(const std::array<float, 3>& xyz) noexcept
            : x{ xyz[0] }, y{ xyz[1] }, z{ xyz[2] } {}

        constexpr Vec3(const Vec3&) noexcept = default;
        constexpr Vec3& operator=(const Vec3&) noexcept = default;

        constexpr Vec3& operator+=(const Vec3& other) noexcept
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        constexpr Vec3& operator-=(const Vec3& other) noexcept
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        constexpr Vec3& operator*=(const Vec3& other) noexcept
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }

        constexpr Vec3& operator/=(const Vec3& other) noexcept
        {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }

        [[nodiscard]] constexpr float& operator[](std::size_t pos) noexcept { return xyz[pos]; }
        [[nodiscard]] constexpr float  operator[](std::size_t pos) const noexcept { return xyz[pos]; }

        struct
        {
            float x, y, z;
        };
        float xyz[3];
    };
    static_assert(std::is_standard_layout_v<Vec3>);
    static_assert(std::is_trivially_copyable_v<Vec3>);
    static_assert(std::is_nothrow_swappable_v<Vec3>);
    static_assert(std::is_nothrow_destructible_v<Vec3>);


    [[nodiscard]] inline constexpr Vec3 operator+(const Vec3& lhs, const Vec3& rhs) noexcept
    {
        return Vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    [[nodiscard]] inline constexpr Vec3 operator+(const Vec3& lhs, float rhs) noexcept
    {
        return lhs + Vec3(rhs);
    }

    [[nodiscard]] inline constexpr Vec3 operator+(float lhs, const Vec3& rhs) noexcept
    {
        return Vec3(lhs) + rhs;
    }

    [[nodiscard]] inline constexpr Vec3 operator-(const Vec3& lhs, const Vec3& rhs) noexcept
    {
        return Vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    [[nodiscard]] inline constexpr Vec3 operator-(const Vec3& lhs, float rhs) noexcept
    {
        return lhs - Vec3(rhs);
    }

    [[nodiscard]] inline constexpr Vec3 operator-(float lhs, const Vec3& rhs) noexcept
    {
        return Vec3(lhs) - rhs;
    }

    [[nodiscard]] inline constexpr Vec3 operator*(const Vec3& lhs, const Vec3& rhs) noexcept
    {
        return Vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
    }

    [[nodiscard]] inline constexpr Vec3 operator*(const Vec3& lhs, float rhs) noexcept
    {
        return lhs * Vec3(rhs);
    }

    [[nodiscard]] inline constexpr Vec3 operator*(float lhs, const Vec3& rhs) noexcept
    {
        return Vec3(lhs) * rhs;
    }

    [[nodiscard]] inline constexpr Vec3 operator/(const Vec3& lhs, const Vec3& rhs) noexcept
    {
        return Vec3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
    }

    [[nodiscard]] inline constexpr Vec3 operator/(const Vec3& lhs, float rhs) noexcept
    {
        return lhs / Vec3(rhs);
    }

    [[nodiscard]] inline constexpr Vec3 operator/(float lhs, const Vec3& rhs) noexcept
    {
        return Vec3(lhs) / rhs;
    }

    [[nodiscard]] inline constexpr bool operator==(const Vec3& lhs, const Vec3& rhs) noexcept
    {
        return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
    }

    [[nodiscard]] inline constexpr bool operator!=(const Vec3& lhs, const Vec3& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    [[nodiscard]] inline /*constexpr*/ float norm(const Vec3& v) noexcept
    {
        // NOTE: currently msvc doesn't accept std::sqrt or std::hypot as constexpr functions

        //return std::sqrt(dot(v, v));
        return std::hypot(v.x, v.y, v.z);
    }

    [[nodiscard]] inline constexpr Vec3 abs(const Vec3& v) noexcept
    {
        return Vec3{ std::abs(v.x), std::abs(v.y), std::abs(v.z) };
    }

    [[nodiscard]] inline constexpr float dot(const Vec3& lhs, const Vec3& rhs) noexcept
    {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
    }

    [[nodiscard]] inline constexpr Vec3 cross(const Vec3& lhs, const Vec3& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        // TODO: impl
        return Vec3(0);
#elif defined(DRAKO_ARCH_INTEL)
        const auto temp = _mm_sub_ps(
            _mm_mul_ps(lhs, _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(1, 2, 0, 3))),
            _mm_mul_ps(rhs, _mm_shuffle_ps(lhs, lhs, _MM_SHUFFLE(1, 2, 0, 3))));
        return _mm_shuffle_ps(temp, temp, _MM_SHUFFLE(1, 2, 0, 3));
#else
#error Missing implementation
#endif
    }

    [[nodiscard]] inline constexpr Vec3 max(const Vec3& lhs, const Vec3& rhs) noexcept
    {
        return Vec3{ std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y), std::max(lhs.z, rhs.z) };
    }

    [[nodiscard]] inline constexpr Vec3 min(const Vec3& lhs, const Vec3& rhs) noexcept
    {
        return Vec3{ std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y), std::min(lhs.z, rhs.z) };
    }

    /// @brief Squared vector magnitude.
    [[nodiscard]] inline constexpr float sqrd_norm(Vec3 v) noexcept
    {
        return dot(v, v);
    }

    [[nodiscard]] inline constexpr float angle_between(Vec3 lhs, Vec3 rhs) noexcept;

    /// @brief Linearly interpolates between two vectors by t in range [0, 1].
    [[nodiscard]] inline constexpr Vec3 lerp(Vec3 v1, Vec3 v2, float t) noexcept
    {
        return (v2 - v1) * t + v1;
    }

    /// @brief Linearly interpolates between vectors.
    [[nodiscard]] inline constexpr Vec3 lerp_unclamped(Vec3 v1, Vec3 v2, float t) noexcept
    {
        return (v2 - v1) * t + v1;
    }

    /// @brief Spherically interpolates between vectors.
    [[nodiscard]] inline constexpr Vec3 slerp(Vec3 v1, Vec3 v2, float t) noexcept;


    [[nodiscard]] inline constexpr Vec3 project_on_vector(Vec3 v, Vec3 n) noexcept;

    [[nodiscard]] inline constexpr Vec3 project_on_plane(Vec3 v, Vec3 plane_normal) noexcept;

    [[nodiscard]] inline constexpr Vec3 reflect_on_plane(Vec3 v, Vec3 plane_normal) noexcept;

    [[nodiscard]] inline constexpr Vec3 normalize(Vec3 v) noexcept;

} // namespace drako

#endif // !DRAKO_VECTOR3_HPP