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

#if !defined(DRAKO_API_SIMD)
#include <intrin.h>
#endif

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/core/preprocessor/platform_macros.hpp"
#include "drako/math/utility.hpp"

namespace drako
{
    class vec3;
    class norm3;

    /// @brief Packed vector with three single-precision scalar components.
    class alignas(sizeof(float) * 4) vec3
    {
    public:
        DRAKO_FORCE_INLINE constexpr explicit vec3() noexcept;
        DRAKO_FORCE_INLINE constexpr explicit vec3(float) noexcept;
        DRAKO_FORCE_INLINE constexpr explicit vec3(float, float, float) noexcept;
        DRAKO_FORCE_INLINE constexpr explicit vec3(const std::array<float, 3>&) noexcept;

        DRAKO_FORCE_INLINE constexpr vec3(const vec3&) noexcept = default;
        DRAKO_FORCE_INLINE constexpr vec3& operator=(const vec3&) noexcept = default;

        DRAKO_NODISCARD constexpr vec3& operator+=(float) noexcept;
        DRAKO_NODISCARD constexpr vec3& operator+=(const vec3&) noexcept;

        DRAKO_NODISCARD constexpr vec3& operator-=(float) noexcept;
        DRAKO_NODISCARD constexpr vec3& operator-=(const vec3&) noexcept;

        DRAKO_NODISCARD constexpr vec3& operator*=(float) noexcept;
        DRAKO_NODISCARD constexpr vec3& operator*=(const vec3&) noexcept;

        DRAKO_NODISCARD constexpr vec3& operator/=(float) noexcept;
        DRAKO_NODISCARD constexpr vec3& operator/=(const vec3&) noexcept;

        friend constexpr vec3 operator+(const vec3&, float) noexcept;
        friend constexpr vec3 operator+(float, const vec3&) noexcept;
        friend constexpr vec3 operator+(const vec3&, const vec3&) noexcept;

        friend constexpr vec3 operator-(const vec3&, float) noexcept;
        friend constexpr vec3 operator-(float, const vec3&) noexcept;
        friend constexpr vec3 operator-(const vec3&, const vec3&) noexcept;

        friend constexpr vec3 operator*(const vec3&, float)noexcept;
        friend constexpr vec3 operator*(float, const vec3&)noexcept;
        friend constexpr vec3 operator*(const vec3&, const vec3&)noexcept;

        friend constexpr vec3 operator/(const vec3&, float) noexcept;
        friend constexpr vec3 operator/(float, const vec3&) noexcept;
        friend constexpr vec3 operator/(const vec3&, const vec3&) noexcept;

        DRAKO_NODISCARD constexpr float& operator[](size_t) noexcept;
        DRAKO_NODISCARD constexpr float  operator[](size_t) const noexcept;

        friend constexpr bool operator==(const vec3&, const vec3&) noexcept;
        friend constexpr bool operator!=(const vec3&, const vec3&) noexcept;

    private:
#if !defined(DRAKO_API_SIMD)
        float _xyz[3];

#elif defined(DRAKO_ARCH_INTEL)

        __m128 _xyzw;

        DRAKO_FORCE_INLINE constexpr vec3(const __m128 floats) noexcept
            : _xyzw(floats)
        {
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr operator __m128() const noexcept { return _xyzw; }

#else
#error Missing implementation
#endif
    };
    static_assert(std::is_standard_layout_v<vec3>);
    static_assert(std::is_trivially_copyable_v<vec3>);
    static_assert(std::is_nothrow_swappable_v<vec3>);
    static_assert(std::is_nothrow_destructible_v<vec3>);


    DRAKO_FORCE_INLINE constexpr vec3::vec3() noexcept
#if !defined(DRAKO_API_SIMD)
        : _xyz{}
    {
    }
#else
#error Missing implementation
#endif

    DRAKO_FORCE_INLINE constexpr vec3::vec3(float f) noexcept
#if !defined(DRAKO_API_SIMD)
        : _xyz{ f, f, f }
    {
    }
#elif defined(DRAKO_ARCH_INTEL)
        : _xyzw(_mm_set_ps1(f))
#else
#error Missing implementation
#endif

    DRAKO_FORCE_INLINE constexpr vec3::vec3(float x, float y, float z) noexcept
#if !defined(DRAKO_API_SIMD)
        : _xyz{ x, y, z }
    {
    }
#elif defined(DRAKO_ARCH_INTEL)
        : _xyzw(_mm_set_ps(x, y, z, 0))
#else
#error Missing implementation
#endif

    DRAKO_FORCE_INLINE constexpr vec3::vec3(const std::array<float, 3>& xyz) noexcept
#if !defined(DRAKO_API_SIMD)
        : _xyz
    {
        xyz[0], xyz[1], xyz[2]
    }
#elif defined(DRAKO_ARCH_INTEL)
        : _xyzw(_mm_set_ps(xyz[0], xyz[1], xyz[2], 0))
#else
#error Missing implementation
#endif
    {
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3& vec3::operator+=(float f) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xyz[0] += f;
        _xyz[1] += f;
        _xyz[2] += f;
#elif defined(DRAKO_ARCH_INTEL)
        _xyzw = _mm_add_ps(_xyzw, _mm_set_ps1(f));
#else
#error Missing implementation
#endif
        return *this;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3& vec3::operator+=(const vec3& v) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xyz[0] += v[0];
        _xyz[1] += v[1];
        _xyz[2] += v[2];
#elif defined(DRAKO_ARCH_INTEL)
        _xyzw = _mm_add_ps(_xyzw, v);
#else
#error Missing implementation
#endif
        return *this;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3& vec3::operator-=(float f) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xyz[0] -= f;
        _xyz[1] -= f;
        _xyz[2] -= f;
#elif defined(DRAKO_ARCH_INTEL)
        _xyzw = _mm_sub_ps(_xyzw, _mm_set_ps1(f));
#else
#error Missing implementation
#endif
        return *this;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3& vec3::operator-=(const vec3& v) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xyz[0] -= v[0];
        _xyz[1] -= v[1];
        _xyz[2] -= v[2];
#elif defined(DRAKO_ARCH_INTEL)
        _xyzw = _mm_sub_ps(_xyzw, v);
#else
#error Missing implementation
#endif
        return *this;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3& vec3::operator*=(float f) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xyz[0] *= f;
        _xyz[1] *= f;
        _xyz[2] *= f;
#else
#error Missing implementation
#endif
        return *this;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3& vec3::operator*=(const vec3& v) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xyz[0] *= v[0];
        _xyz[1] *= v[1];
        _xyz[2] *= v[2];
#else
#error Missing implementation
#endif
        return *this;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3& vec3::operator/=(float f) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xyz[0] /= f;
        _xyz[1] /= f;
        _xyz[2] /= f;
#else
#error Missing implementation
#endif
        return *this;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3& vec3::operator/=(const vec3& v) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xyz[0] /= v[0];
        _xyz[1] /= v[1];
        _xyz[2] /= v[2];
#else
#error Missing implementation
#endif
        return *this;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3 operator+(const vec3& lhs, const vec3& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec3(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]);
#elif defined(DRAKO_ARCH_INTEL)
        return _mm_add_ps(lhs, rhs);
#else
#error Missing implementation
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3 operator+(const vec3& lhs, float rhs) noexcept
    {
        return lhs + vec3(rhs);
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3 operator+(float lhs, const vec3& rhs) noexcept
    {
        return vec3(lhs) + rhs;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3 operator-(const vec3& lhs, const vec3& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec3(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]);
#elif defined(DRAKO_ARCH_INTEL)
        return _mm_sub_ps(lhs, rhs);
#else
#error Missing implementation
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3 operator-(const vec3& lhs, float rhs) noexcept
    {
        return lhs - vec3(rhs);
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3 operator-(float lhs, const vec3& rhs) noexcept
    {
        return vec3(lhs) - rhs;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3 operator*(const vec3& lhs, const vec3& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec3(lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2]);
#elif defined(DRAKO_ARCH_INTEL)
        return _mm_mul_ps(lhs, rhs);
#else
#error Missing implementation
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3 operator*(const vec3& lhs, float rhs) noexcept
    {
        return lhs * vec3(rhs);
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3 operator*(float lhs, const vec3& rhs) noexcept
    {
        return vec3(lhs) * rhs;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3 operator/(const vec3& lhs, const vec3& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec3(lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2]);
#elif defined(DRAKO_ARCH_INTEL)
        return _mm_div_ps(lhs, v);
#else
#error Missing implementation
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3 operator/(const vec3& lhs, float rhs) noexcept
    {
        return lhs / vec3(rhs);
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3 operator/(float lhs, const vec3& rhs) noexcept
    {
        return vec3(lhs) / rhs;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr bool operator==(const vec3& lhs, const vec3& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && (lhs[2] == rhs[2]);
#elif defined(DRAKO_ARCH_INTEL)
        const __mm128 cmp_mask = _mm_cmpeq_ps(lhs, v);
        return (_mm_movemask_ps(cmp_mask) == 0xFFFF);
#else
#error Missing implementation
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr bool operator!=(const vec3& lhs, const vec3& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return !(lhs == rhs);
#else
#error Missing implementation
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float vec3::operator[](size_t idx) const noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return _xyz[idx];
#elif defined(DRAKO_ARCH_INTEL)
        return _xyzw.m128_f32[idx];
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float& vec3::operator[](size_t idx) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return _xyz[idx];
#else
        return _xyzw.m128_f32[idx];
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE /*constexpr*/ float
    norm(const vec3& v) noexcept
    {
        // NOTE: currently msvc doesn't accept std::sqrt or std::hypot as constexpr functions

        //return std::sqrt(dot(v, v));
        return std::hypot(v[0], v[1], v[2]);
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3
    abs(const vec3& v) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec3{ std::abs(v[0]), std::abs(v[1]), std::abs(v[2]) };
#elif defined(DRAKO_ARCH_INTEL)
#error Missing implementation
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3
    max(const vec3& lhs, const vec3& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec3{ std::max(lhs[0], rhs[0]), std::max(lhs[1], rhs[1]), std::max(lhs[2], rhs[2]) };
#elif defined(DRAKO_ARCH_INTEL)
        return _mm_max_ps(lhs, rhs);
#else
#error Missing implementation
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float
    dot(const vec3& lhs, const vec3& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
#else
#error Missing implementation
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3
    cross(const vec3& lhs, const vec3& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        // TODO: impl
        return vec3(0);
#elif defined(DRAKO_ARCH_INTEL)
        const auto temp = _mm_sub_ps(
            _mm_mul_ps(lhs, _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(1, 2, 0, 3))),
            _mm_mul_ps(rhs, _mm_shuffle_ps(lhs, lhs, _MM_SHUFFLE(1, 2, 0, 3))));
        return _mm_shuffle_ps(temp, temp, _MM_SHUFFLE(1, 2, 0, 3));
#else
#error Missing implementation
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3
    max(const vec3& lhs, const vec3& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec3{ std::max(lhs[0], rhs[0]), std::max(lhs[1], rhs[1]), std::max(lhs[2], rhs[2]) };
#elif defined(DRAKO_ARCH_INTEL)
        return _mm_max_ps(lhs, rhs);
#else
#error Missing implementation
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3
    min(const vec3& lhs, const vec3& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec3{ std::min(lhs[0], rhs[0]), std::min(lhs[1], rhs[1]), std::min(lhs[2], rhs[2]) };
#elif defined(DRAKO_ARCH_INTEL)
        return _mm_min_ps(lhs, rhs);
#else
#error Missing implementation
#endif
    }


    DRAKO_FORCE_INLINE std::ostream&
    operator<<(std::ostream& os, const vec3& v) noexcept
    {
        return os << v[0] << ' ' << v[1] << ' ' << v[2];
    }

    DRAKO_FORCE_INLINE std::istream& operator>>(std::istream& is, vec3& v) noexcept
    {
        return is >> v[0] >> v[1] >> v[2];
    }


    /// @brief Squared vector magnitude.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float sqrd_norm(vec3 v) noexcept
    {
        return dot(v, v);
    }

    DRAKO_NODISCARD inline constexpr float angle_between(vec3 lhs, vec3 rhs) noexcept;

    /// @brief Linearly interpolates between two vectors by t in range [0, 1].
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3 lerp(vec3 v1, vec3 v2, float t) noexcept
    {
        t = clamp_zero_one(t);
        return (v2 - v1) * t + v1;
    }

    /// @brief Linearly interpolates between vectors.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec3 lerp_unclamped(vec3 v1, vec3 v2, float t) noexcept
    {
        return (v2 - v1) * t + v1;
    }

    /// @brief Spherically interpolates between vectors.
    DRAKO_NODISCARD inline constexpr vec3 slerp(vec3 v1, vec3 v2, float t) noexcept;


    DRAKO_NODISCARD inline constexpr vec3 project_on_vector(vec3 v, vec3 n) noexcept;

    DRAKO_NODISCARD inline constexpr vec3 project_on_plane(vec3 v, vec3 plane_normal) noexcept;

    DRAKO_NODISCARD inline constexpr vec3 reflect_on_plane(vec3 v, vec3 plane_normal) noexcept;

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr norm3 normalize(vec3 v) noexcept;



    // Unit vector with three component.
    //
    class alignas(sizeof(float) * 4) norm3 : public vec3
    {
    public:
        constexpr explicit norm3(vec3 v_) noexcept
            : vec3(normalize(v_))
        {
        }

    private:
    };
    static_assert(std::is_standard_layout_v<norm3>);
    static_assert(std::is_trivially_copyable_v<norm3>);
    static_assert(std::is_nothrow_swappable_v<norm3>);
    static_assert(std::is_nothrow_destructible_v<norm3>);


    // Normalizes the vector.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr norm3 normalize(vec3 v) noexcept
    {
        return static_cast<norm3>(v / vec3(norm(v)));
    }


    /// @brief Packed vector with three 32bit integer components.
    class alignas(sizeof(float) * 4) ivec3
    {
    public:
        DRAKO_FORCE_INLINE constexpr explicit ivec3() noexcept;
        DRAKO_FORCE_INLINE constexpr explicit ivec3(int32_t) noexcept;
        DRAKO_FORCE_INLINE constexpr explicit ivec3(int32_t, int32_t, int32_t) noexcept;
        DRAKO_FORCE_INLINE constexpr explicit ivec3(int32_t[3]) noexcept;

        DRAKO_FORCE_INLINE constexpr ivec3(const ivec3&) noexcept = default;
        DRAKO_FORCE_INLINE constexpr ivec3& operator=(const ivec3&) noexcept = default;

        DRAKO_NODISCARD constexpr ivec3& operator+=(int32_t) noexcept;
        DRAKO_NODISCARD constexpr ivec3& operator+=(const ivec3&) noexcept;

        DRAKO_NODISCARD constexpr ivec3& operator-=(int32_t) noexcept;
        DRAKO_NODISCARD constexpr ivec3& operator-=(const ivec3&) noexcept;

        DRAKO_NODISCARD constexpr ivec3& operator*=(int32_t) noexcept;
        DRAKO_NODISCARD constexpr ivec3& operator*=(const ivec3&) noexcept;

        DRAKO_NODISCARD constexpr ivec3& operator/=(int32_t) noexcept;
        DRAKO_NODISCARD constexpr ivec3& operator/=(const ivec3&) noexcept;

        friend constexpr ivec3 operator+(const ivec3&, int32_t) noexcept;
        friend constexpr ivec3 operator+(int32_t, const ivec3&) noexcept;
        friend constexpr ivec3 operator+(const ivec3&, const ivec3&) noexcept;

        friend constexpr ivec3 operator-(const ivec3&, int32_t) noexcept;
        friend constexpr ivec3 operator-(int32_t, const ivec3&) noexcept;
        friend constexpr ivec3 operator-(const ivec3&, const ivec3&) noexcept;

        friend constexpr ivec3 operator*(const ivec3&, int32_t) noexcept;
        friend constexpr ivec3 operator*(int32_t, const ivec3&)noexcept;
        friend constexpr ivec3 operator*(const ivec3&, const ivec3&)noexcept;

        friend constexpr ivec3 operator/(const ivec3&, int32_t) noexcept;
        friend constexpr ivec3 operator/(int32_t, const ivec3&) noexcept;
        friend constexpr ivec3 operator/(const ivec3&, const ivec3&) noexcept;

        DRAKO_NODISCARD constexpr int32_t& operator[](size_t) noexcept;
        DRAKO_NODISCARD constexpr int32_t  operator[](size_t) const noexcept;

        friend constexpr bool operator==(const ivec3&, const ivec3&) noexcept;
        friend constexpr bool operator!=(const ivec3&, const ivec3&) noexcept;

    private:
#if !defined(DRAKO_API_SIMD)
        int32_t _xyz[3];

#else
#error Missing implementation
#endif
    };

    DRAKO_FORCE_INLINE constexpr ivec3::ivec3() noexcept
        : _xyz{ 0, 0, 0 }
    {
    }

    DRAKO_FORCE_INLINE constexpr ivec3::ivec3(int32_t xyz) noexcept
        : _xyz{ xyz, xyz, xyz }
    {
    }

    DRAKO_FORCE_INLINE constexpr ivec3::ivec3(int32_t x, int32_t y, int32_t z) noexcept
        : _xyz{ x, y, z }
    {
    }

    DRAKO_FORCE_INLINE constexpr ivec3::ivec3(int32_t xyz[3]) noexcept
        : _xyz{ xyz[0], xyz[1], xyz[2] }
    {
    }

    DRAKO_FORCE_INLINE constexpr ivec3& ivec3::operator+=(int32_t rhs) noexcept
    {
        _xyz[0] += rhs;
        _xyz[1] += rhs;
        _xyz[2] += rhs;
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr ivec3& ivec3::operator+=(const ivec3& rhs) noexcept
    {
        _xyz[0] += rhs[0];
        _xyz[1] += rhs[1];
        _xyz[2] += rhs[2];
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr ivec3& ivec3::operator-=(int32_t rhs) noexcept
    {
        _xyz[0] -= rhs;
        _xyz[1] -= rhs;
        _xyz[2] -= rhs;
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr ivec3& ivec3::operator-=(const ivec3& rhs) noexcept
    {
        _xyz[0] -= rhs[0];
        _xyz[1] -= rhs[1];
        _xyz[2] -= rhs[2];
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr ivec3& ivec3::operator*=(int32_t rhs) noexcept
    {
        _xyz[0] *= rhs;
        _xyz[1] *= rhs;
        _xyz[2] *= rhs;
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr ivec3& ivec3::operator*=(const ivec3& rhs) noexcept
    {
        _xyz[0] *= rhs[0];
        _xyz[1] *= rhs[1];
        _xyz[2] *= rhs[2];
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr ivec3& ivec3::operator/=(int32_t rhs) noexcept
    {
        _xyz[0] /= rhs;
        _xyz[1] /= rhs;
        _xyz[2] /= rhs;
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr ivec3& ivec3::operator/=(const ivec3& rhs) noexcept
    {
        _xyz[0] /= rhs[0];
        _xyz[1] /= rhs[1];
        _xyz[2] /= rhs[2];
        return *this;
    }


} // namespace drako

#endif // !DRAKO_VECTOR3_HPP