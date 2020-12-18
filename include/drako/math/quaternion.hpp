#pragma once
#ifndef DRAKO_QUATERNION_HPP
#define DRAKO_QUATERNION_HPP

#include "drako/core/platform.hpp"
#include "drako/math/vector4.hpp"

#include <type_traits>

namespace drako
{
    /// @brief Hamilton Quaternion.
    union alignas(sizeof(float) * 4) Quat
    {
        /// @brief Constructs the identity Quaternion.
        explicit constexpr Quat() noexcept
            : x{ 0.f }, y{ 0.f }, z{ 0.f }, w{ 1.f } {}

        explicit constexpr Quat(float x, float y, float z, float w) noexcept
            : x{ x }, y{ y }, z{ z }, w{ w } {}

        explicit constexpr Quat(const Vec4& xyzw) noexcept
            : x{ xyzw.x }, y{ xyzw.y }, z{ xyzw.z }, w{ xyzw.w } {}

        constexpr Quat(const Quat&) noexcept = default;
        constexpr Quat& operator=(const Quat&) noexcept = default;

        friend constexpr Quat operator*(Quat lhs, Quat rhs) noexcept;

        [[nodiscard]] friend constexpr bool operator==(Quat, Quat) noexcept = default;
        [[nodiscard]] friend constexpr bool operator!=(Quat, Quat) noexcept = default;

        // Normalizes the Quaternion
        constexpr void normalize() noexcept;

        // Returns the Quaternion normalized.
        [[nodiscard]] constexpr Quat normalized() const noexcept;

        struct
        {
            float x, y, w, z;
        };
        float xyzw[4];
    };
    static_assert(std::is_standard_layout_v<Quat>);
    static_assert(std::is_trivially_copyable_v<Quat>);
    static_assert(std::is_nothrow_swappable_v<Quat>);
    static_assert(std::is_nothrow_destructible_v<Quat>);


    // Computes the norm of the Quaternion.
    [[nodiscard]] inline float norm(Quat q) noexcept
    {
        return norm(Vec4{ q.xyzw });
    }

    // Computes the conjugate of the Quaternion.
    [[nodiscard]] constexpr Quat conjugate(Quat q) noexcept
    {
        return Quat{ -q.x, -q.y, -q.z, q.w };
    }

    // Computes the inverse of the Quaternion.
    [[nodiscard]] inline constexpr Quat inverse(Quat q) noexcept;

    [[nodiscard]] inline constexpr bool normalized(Quat q) noexcept;

    // Checks if the Quaternion represents the same rotation.
    [[nodiscard]] inline constexpr bool equivalent(Quat q1, Quat q2) noexcept;

} // namespace drako

#endif // !DRAKO_QUATERNION_HPP