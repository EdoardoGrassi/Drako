#pragma once
#ifndef DRAKO_IVEC2_HPP
#define DRAKO_IVEC2_HPP

/// @file
///
/// @brief  Vector class with two 32bit integer components and utility functions.
/// @author Grassi Edoardo
/// @date   Last update: 06-09-2019

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string>
#include <type_traits>

#include "drako/core/preprocessor/compiler_macros.hpp"

namespace drako
{
    /// @brief Packed vector with two 32bit integer components.
    class alignas(sizeof(int32_t) * 2) ivec2 final
    {
    public:

        DRAKO_FORCE_INLINE constexpr explicit ivec2() noexcept;
        DRAKO_FORCE_INLINE constexpr explicit ivec2(int32_t) noexcept;
        DRAKO_FORCE_INLINE constexpr explicit ivec2(int32_t, int32_t) noexcept;
        DRAKO_FORCE_INLINE constexpr explicit ivec2(const std::array<int32_t, 2>&) noexcept;

        DRAKO_FORCE_INLINE constexpr ivec2(const ivec2&) noexcept = default;
        DRAKO_FORCE_INLINE constexpr ivec2& operator=(const ivec2&) noexcept = default;

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2& operator+=(const ivec2&) noexcept;
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2& operator-=(const ivec2&) noexcept;
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2& operator*=(const ivec2&) noexcept;
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2& operator/=(const ivec2&) noexcept;

        DRAKO_FORCE_INLINE constexpr friend ivec2 operator+(const ivec2&, const ivec2&) noexcept;
        DRAKO_FORCE_INLINE constexpr friend ivec2 operator-(const ivec2&, const ivec2&) noexcept;
        DRAKO_FORCE_INLINE constexpr friend ivec2 operator*(const ivec2&, const ivec2&) noexcept;
        DRAKO_FORCE_INLINE constexpr friend ivec2 operator/(const ivec2&, const ivec2&) noexcept;

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr int32_t& operator[](size_t) noexcept;
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr int32_t  operator[](size_t) const noexcept;

        DRAKO_FORCE_INLINE constexpr friend bool operator==(const ivec2&, const ivec2&) noexcept;
        DRAKO_FORCE_INLINE constexpr friend bool operator==(const ivec2&, const ivec2&) noexcept;

        friend std::istream& operator>>(std::istream&, ivec2&);
        friend std::ostream& operator<<(std::ostream&, const ivec2&);

        DRAKO_NODISCARD DRAKO_FORCE_INLINE friend float norm(ivec2 v) noexcept
        {
            return static_cast<float>(std::sqrt(dot(v, v)));
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr friend int32_t sqrd_norm(ivec2 v) noexcept
        {
            return dot(v, v);
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr friend int32_t dot(ivec2 lhs, ivec2 rhs) noexcept
        {
            return lhs[0] * rhs[0] + lhs[1] * rhs[1];
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr friend auto max(ivec2 lhs, ivec2 rhs) noexcept
        {
            return ivec2(std::max(lhs[0], rhs[0]), std::max(lhs[1], rhs[1]));
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr friend auto min(ivec2 lhs, ivec2 rhs) noexcept
        {
            return ivec2(std::min(lhs[0], rhs[0]), std::min(lhs[1], rhs[1]));
        }

    private:

        #if !defined (DRAKO_API_SIMD)
        int32_t _xy[2];
        #else
        #error Explicit SIMD not supported.
        #endif
    };
    static_assert(std::is_standard_layout_v<ivec2>);
    static_assert(std::is_trivially_copyable_v<ivec2>);
    static_assert(std::is_nothrow_swappable_v<ivec2>);
    static_assert(std::is_nothrow_destructible_v<ivec2>);


    DRAKO_FORCE_INLINE constexpr ivec2::ivec2() noexcept
        : _xy{}
    {
    }

    DRAKO_FORCE_INLINE constexpr ivec2::ivec2(int32_t xy) noexcept
        : _xy{ xy, xy }
    {
    }

    DRAKO_FORCE_INLINE constexpr ivec2::ivec2(int32_t x, int32_t y) noexcept
        #if !defined (DRAKO_API_SIMD)
        : _xy{ x, y }
        #else

        #endif
    {
    }

    DRAKO_FORCE_INLINE constexpr ivec2::ivec2(const std::array<int32_t, 2> & xy) noexcept
        #if !defined (DRAKO_API_SIMD)
        : _xy{ xy[0], xy[1] }
        #else

        #endif
    {
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr int32_t& ivec2::operator[](size_t idx) noexcept
    {
        return _xy[idx];
    }
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr int32_t ivec2::operator[](size_t idx) const noexcept
    {
        return _xy[idx];
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2 operator+(const ivec2& lhs, const ivec2& rhs) noexcept
    {
        return ivec2(lhs[0] + rhs[0], lhs[1] + rhs[1]);
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2 operator+(const ivec2& lhs, int32_t rhs) noexcept { return lhs + ivec2(rhs); }
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2 operator+(int32_t lhs, const ivec2& rhs) noexcept { return ivec2(lhs) + rhs; }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2 operator-(const ivec2& lhs, const ivec2& rhs) noexcept
    {
        return ivec2(lhs[0] - rhs[0], lhs[1] - rhs[1]);
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2 operator-(const ivec2& lhs, int32_t rhs) noexcept { return lhs - ivec2(rhs); }
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2 operator-(int32_t lhs, const ivec2& rhs) noexcept { return ivec2(lhs) - rhs; }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2 operator*(const ivec2& lhs, const ivec2& rhs) noexcept
    {
        return ivec2(lhs[0] * rhs[0], lhs[1] * rhs[1]);
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2 operator*(const ivec2& lhs, int32_t rhs) noexcept { return lhs * ivec2(rhs); }
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2 operator*(int32_t lhs, const ivec2& rhs) noexcept { return ivec2(lhs) * rhs; }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2 operator/(const ivec2& lhs, const ivec2& rhs) noexcept
    {
        return ivec2(lhs[0] / rhs[0], lhs[1] / rhs[1]);
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2 operator/(const ivec2& lhs, int32_t rhs) noexcept { return lhs / ivec2(rhs); }
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr ivec2 operator/(int32_t lhs, const ivec2& rhs) noexcept { return ivec2(lhs) / rhs; }

    DRAKO_NODISCARD inline std::string to_string(const ivec2& v)
    {
        return std::string("[" + std::to_string(v[0]) + ", " + std::to_string(v[1]) + "]");
    }
} // namespace drako

#endif // !DRAKO_IVEC2_HPP