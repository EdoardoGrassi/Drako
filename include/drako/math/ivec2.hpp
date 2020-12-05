#pragma once
#ifndef DRAKO_IVEC2_HPP
#define DRAKO_IVEC2_HPP

/// @file
///
/// @brief  Vector class with two 32bit integer components and utility functions.
/// @author Grassi Edoardo
/// @date   Last update: 03-12-2020

#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <type_traits>

namespace drako
{
    /// @brief Packed vector with two 32bit integer components.
    struct alignas(sizeof(std::int32_t) * 2) ivec2 final
    {
        constexpr explicit ivec2() noexcept
            : xy{ 0, 0 } {}

        constexpr explicit ivec2(std::int32_t i) noexcept
            : xy{ i, i } {}

        constexpr explicit ivec2(std::int32_t x, std::int32_t y) noexcept
            : xy{ x, y } {}

        constexpr explicit ivec2(const std::array<std::int32_t, 2>& xy) noexcept
            : xy{ xy[0], xy[1] } {}

        constexpr ivec2(const ivec2&) noexcept = default;
        constexpr ivec2& operator=(const ivec2&) noexcept = default;

        constexpr ivec2& operator+=(ivec2 other) noexcept
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        constexpr ivec2& operator-=(ivec2 other) noexcept
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        constexpr ivec2& operator*=(ivec2 other) noexcept
        {
            x *= other.x;
            y *= other.y;
            return *this;
        }

        constexpr ivec2& operator/=(ivec2 other) noexcept
        {
            x /= other.x;
            y /= other.y;
            return *this;
        }

        [[nodiscard]] constexpr std::int32_t& operator[](std::size_t i) noexcept { return xy[i]; }
        [[nodiscard]] constexpr std::int32_t  operator[](std::size_t i) const noexcept { return xy[i]; }

        union
        {
            struct
            {
                std::int32_t x, y;
            };
            std::int32_t xy[2];
        };
    };
    static_assert(std::is_standard_layout_v<ivec2>);
    static_assert(std::is_trivially_copyable_v<ivec2>);
    static_assert(std::is_nothrow_swappable_v<ivec2>);
    static_assert(std::is_nothrow_destructible_v<ivec2>);

    [[nodiscard]] constexpr ivec2 operator+(ivec2 lhs, ivec2 rhs) noexcept
    {
        return ivec2{ lhs.x + rhs.x, lhs.y + rhs.y };
    }

    [[nodiscard]] constexpr ivec2 operator-(ivec2 lhs, ivec2 rhs) noexcept
    {
        return ivec2{ lhs.x - rhs.x, lhs.y - rhs.y };
    }

    [[nodiscard]] constexpr ivec2 operator*(ivec2 lhs, ivec2 rhs) noexcept
    {
        return ivec2{ lhs.x * rhs.x, lhs.y * rhs.y };
    }

    [[nodiscard]] constexpr ivec2 operator/(ivec2 lhs, ivec2 rhs) noexcept
    {
        return ivec2{ lhs.x / rhs.x, lhs.y / rhs.y };
    }

    [[nodiscard]] constexpr bool operator==(ivec2 lhs, ivec2 rhs) noexcept
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    [[nodiscard]] constexpr bool operator!=(ivec2 lhs, ivec2 rhs) noexcept
    {
        return !(lhs == rhs);
    }

    [[nodiscard]] inline constexpr std::int32_t dot(ivec2 lhs, ivec2 rhs) noexcept
    {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y);
    }

    [[nodiscard]] inline float norm(ivec2 v) noexcept
    {
        return static_cast<float>(std::sqrt(dot(v, v)));
    }

    [[nodiscard]] inline constexpr std::int32_t sqrd_norm(ivec2 v) noexcept
    {
        return dot(v, v);
    }

    [[nodiscard]] inline constexpr auto max(ivec2 lhs, ivec2 rhs) noexcept
    {
        return ivec2(std::max(lhs[0], rhs[0]), std::max(lhs[1], rhs[1]));
    }

    [[nodiscard]] inline constexpr auto min(ivec2 lhs, ivec2 rhs) noexcept
    {
        return ivec2(std::min(lhs[0], rhs[0]), std::min(lhs[1], rhs[1]));
    }

    inline std::ostream& operator<<(std::ostream& os, const ivec2& v)
    {
        return os << '(' << v.x << ',' << v.y << ')';
    }

} // namespace drako

#endif // !DRAKO_IVEC2_HPP