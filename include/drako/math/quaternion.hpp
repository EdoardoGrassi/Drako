#pragma once
#ifndef DRAKO_QUATERNION_HPP
#define DRAKO_QUATERNION_HPP

#include "drako/core/platform.hpp"
#include "drako/math/vector4.hpp"

#include <type_traits>

#if defined(DRAKO_API_SIMD)
#include <intrin.h>
#endif

namespace drako
{
    /// @brief Hamilton quaternion.
    union alignas(sizeof(float) * 4) quat
    {
        /// @brief Constructs the identity quaternion. 
        explicit constexpr quat() noexcept
            : x{ 0.f }, y{ 0.f }, z{ 0.f }, w{ 1.f } {}

        explicit constexpr quat(float x, float y, float z, float w) noexcept
            : x{ x }, y{ y }, z{ z }, w{ w } {}

        explicit constexpr quat(const Vec4& xyzw) noexcept
            : x{ xyzw.x }, y{ xyzw.y }, z{ xyzw.z }, w{ xyzw.w } {}

        constexpr quat(const quat&) noexcept = default;
        constexpr quat& operator=(const quat&) noexcept = default;

        [[nodiscard]] explicit operator mat4x4() const noexcept;

        friend constexpr quat operator*(quat lhs, quat rhs) noexcept;

        [[nodiscard]] friend constexpr bool operator==(quat, quat) noexcept = default;
        [[nodiscard]] friend constexpr bool operator!=(quat, quat) noexcept = default;

        // Normalizes the quaternion
        constexpr void normalize() noexcept;

        // Returns the quaternion normalized.
        [[nodiscard]] constexpr quat normalized() const noexcept;

        struct
        {
            float x, y, w, z;
        };
        float xyzw[4];
    };
    static_assert(std::is_standard_layout_v<quat>);
    static_assert(std::is_trivially_copyable_v<quat>);
    static_assert(std::is_nothrow_swappable_v<quat>);
    static_assert(std::is_nothrow_destructible_v<quat>);


    // Computes the norm of the quaternion.
    [[nodiscard]] inline float norm(quat q) noexcept
    {
        return norm(Vec4{ q.xyzw });
    }

    // Computes the conjugate of the quaternion.
    [[nodiscard]] constexpr quat conjugate(quat q) noexcept
    {
        return quat{ -q.x, -q.y, -q.z, q.w };
    }

    // Computes the inverse of the quaternion.
    [[nodiscard]] inline constexpr quat inverse(quat q) noexcept;

    [[nodiscard]] inline constexpr bool normalized(quat q) noexcept;

    // Checks if the quaternion represents the same rotation.
    [[nodiscard]] inline constexpr bool equivalent(quat q1, quat q2) noexcept;

} // namespace drako

#endif // !DRAKO_QUATERNION_HPP