#pragma once
#ifndef DRAKO_QUATERNION_HPP
#define DRAKO_QUATERNION_HPP

#include <type_traits>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/math/mat4x4.hpp"
#include "drako/math/vector4.hpp"

#if defined(DRAKO_API_SIMD)
#include <intrin.h>
#endif

namespace drako
{
    // forward declarations
    class mat4x4;

    // Hamilton quaternion.
    class alignas(sizeof(float) * 4) quat
    {
    public:
        DRAKO_FORCE_INLINE explicit constexpr quat(float x, float y, float z, float w) noexcept
            : _xyzw(x, y, z, w)
        {
        }

        DRAKO_FORCE_INLINE explicit constexpr quat(const vec4& xyzw) noexcept
            : _xyzw(xyzw)
        {
        }

        constexpr quat(const quat&) noexcept = default;
        constexpr quat& operator=(const quat&) noexcept = default;

        DRAKO_NODISCARD DRAKO_FORCE_INLINE explicit operator mat4x4() const noexcept
        {
            return mat4x4{};
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE friend constexpr quat
        operator*(quat lhs, quat rhs) noexcept
        {
        }

        // Computes the norm of the quaternion.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr friend float norm(quat q) noexcept
        {
            return norm(q._xyzw);
        }

        // Normalizes the quaternion
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr void normalize() noexcept;

        // Returns the quaternion normalized.
        DRAKO_NODISCARD inline constexpr quat normalized() const noexcept;

        // Computes the conjugate of the quaternion.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr friend quat conjugate(quat q) noexcept
        {
#if !defined(DRAKO_API_SIMD)
            return quat(q._xyzw * vec4(-1.f, -1.f, -1.f, 1.f));
#else
            return _mm_xor_ps(*this, _mm_set_ps(-0.f, -0.f, -0.f, 0));
#endif
        }

        // Computes the inverse of the quaternion.
        DRAKO_NODISCARD inline constexpr quat inverse() const noexcept;

        // The identity quaternion I = (0,0,0,1).
        DRAKO_NODISCARD DRAKO_FORCE_INLINE static constexpr quat identity() noexcept
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


    DRAKO_NODISCARD constexpr bool normalized(quat q) noexcept;


    // Checks if the quaternion represents the same rotation.
    DRAKO_NODISCARD constexpr bool equivalent(quat q1, quat q2) noexcept;


    // Unit lenght Hamilton quaternion.
    class alignas(sizeof(float) * 4) uquat
    {
    public:

        DRAKO_FORCE_INLINE explicit constexpr uquat(float x, float y, float z, float w) noexcept
            : _xyzw(x, y, z, w)
        {
        }

        DRAKO_FORCE_INLINE explicit constexpr uquat(const vec4& xyzw) noexcept
            : _xyzw(xyzw)
        {
        }

        // The identity quaternion I = (0,0,0,1).
        DRAKO_NODISCARD DRAKO_FORCE_INLINE static constexpr uquat identity() noexcept
        {
            return uquat(0.f, 0.f, 0.f, 1.f);
        }

        // Linear interpolation without renormalization between quaternions.
        DRAKO_NODISCARD friend constexpr uquat lerp(uquat q1, uquat q2, float t) noexcept
        {
            return uquat(lerp(q1._xyzw, q2._xyzw, t));
        }

        // Linear interpolation with renormalization between quaternions.
        DRAKO_NODISCARD friend constexpr uquat nlerp(uquat q1, uquat q2, float t) noexcept
        {
            return uquat(lerp(q1._xyzw, q2._xyzw, t));
        }

        // Spherical interpolation between quaternions.
        DRAKO_NODISCARD friend constexpr uquat v_slerp(uquat q1, uquat q2, float t) noexcept
        {
            const float O    = std::acos(dot(q1._xyzw, q2._xyzw));
            const float sinO = std::sin(O);
            return (std::sin(O * (1 - t)) / sinO) * q1 + (std::sin(O * t) / sinO) * q2;
        }

    private:
        vec4 _xyzw;
    };
    static_assert(std::is_standard_layout_v<uquat>);
    static_assert(std::is_trivially_copyable_v<uquat>);

} // namespace drako

#endif // !DRAKO_QUATERNION_HPP