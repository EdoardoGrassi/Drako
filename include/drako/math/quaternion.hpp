#pragma once
#ifndef DRAKO_QUATERNION_HPP
#define DRAKO_QUATERNION_HPP

#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/math/mat4x4.hpp"
#include "drako/math/vector4.hpp"

#include <type_traits>

#if defined(DRAKO_API_SIMD)
#include <intrin.h>
#endif

namespace drako
{
    /// @brief Hamilton quaternion.
    class alignas(sizeof(float) * 4) quat
    {
    public:
        explicit constexpr quat(float x, float y, float z, float w) noexcept
            : _xyzw(x, y, z, w)
        {
        }

        explicit constexpr quat(const vec4& xyzw) noexcept
            : _xyzw(xyzw)
        {
        }

        constexpr quat(const quat&) noexcept = default;
        constexpr quat& operator=(const quat&) noexcept = default;

        [[nodiscard]] explicit operator mat4x4() const noexcept;

        friend constexpr quat operator*(quat lhs, quat rhs) noexcept;

        // Computes the norm of the quaternion.
        [[nodiscard]] friend float norm(quat q) noexcept
        {
            return norm(q._xyzw);
        }

        // Normalizes the quaternion
        constexpr void normalize() noexcept;

        // Returns the quaternion normalized.
        [[nodiscard]] constexpr quat normalized() const noexcept;

        // Computes the conjugate of the quaternion.
        [[nodiscard]] constexpr quat conjugate(quat q) noexcept
        {
#if !defined(DRAKO_API_SIMD)
            return quat(q._xyzw * vec4(-1.f, -1.f, -1.f, 1.f));
#else
            return _mm_xor_ps(*this, _mm_set_ps(-0.f, -0.f, -0.f, 0));
#endif
        }

        // Computes the inverse of the quaternion.
        [[nodiscard]] constexpr quat inverse() const noexcept;

        // The identity quaternion I = (0,0,0,1).
        [[nodiscard]] static constexpr quat identity() noexcept
        {
            return quat(0.f, 0.f, 0.f, 1.f);
        }

    private:
        vec4 _xyzw;
    };
    static_assert(std::is_standard_layout_v<quat>);
    static_assert(std::is_trivially_copyable_v<quat>);
    static_assert(std::is_nothrow_swappable_v<quat>);
    static_assert(std::is_nothrow_destructible_v<quat>);


    [[nodiscard]] constexpr bool normalized(quat q) noexcept;


    // Checks if the quaternion represents the same rotation.
    [[nodiscard]] constexpr bool equivalent(quat q1, quat q2) noexcept;


    /// @brief Unit lenght Hamilton quaternion.
    class alignas(sizeof(float) * 4) uquat
    {
    public:
        explicit constexpr uquat(float x, float y, float z, float w) noexcept
            : _xyzw(x, y, z, w)
        {
        }

        explicit constexpr uquat(const vec4& xyzw) noexcept
            : _xyzw(xyzw)
        {
        }

        // The identity quaternion I = (0,0,0,1).
        [[nodiscard]] static constexpr uquat identity() noexcept
        {
            return uquat(0.f, 0.f, 0.f, 1.f);
        }

        // Linear interpolation without renormalization between quaternions.
        [[nodiscard]] friend constexpr uquat lerp(uquat q1, uquat q2, float t) noexcept
        {
            return uquat(lerp(q1._xyzw, q2._xyzw, t));
        }

        // Linear interpolation with renormalization between quaternions.
        [[nodiscard]] friend constexpr uquat nlerp(uquat q1, uquat q2, float t) noexcept
        {
            return uquat(lerp(q1._xyzw, q2._xyzw, t));
        }

        // Spherical interpolation between quaternions.
        /*[[nodiscard]] friend uquat v_slerp(uquat q1, uquat q2, float t) noexcept;
        {
            const float O    = std::acos(dot(q1._xyzw, q2._xyzw));
            const float sinO = std::sin(O);
            return (std::sin(O * (1 - t)) / sinO) * q1 + (std::sin(O * t) / sinO) * q2;
        }*/

    private:
        vec4 _xyzw;
    };
    static_assert(std::is_standard_layout_v<uquat>);
    static_assert(std::is_trivially_copyable_v<uquat>);

} // namespace drako

#endif // !DRAKO_QUATERNION_HPP