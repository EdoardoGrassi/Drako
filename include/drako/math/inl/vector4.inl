#pragma once
#ifndef DRAKO_VECTOR4_INL
#define DRAKO_VECTOR4_INL

#include "drako/core/preprocessor/compiler_macros.hpp"

namespace drako
{
    DRAKO_FORCE_INLINE constexpr vec4::vec4(float f) noexcept
#if !defined(DRAKO_API_SIMD)
        : _xyzw
    {
        f, f, f, f
    }
#else
        : _xyzw(_mm_set_ps1(f))
#endif
    {
    }

    DRAKO_FORCE_INLINE constexpr vec4::vec4(float x, float y, float z, float w) noexcept
#if !defined(DRAKO_API_SIMD)
        : _xyzw
    {
        x, y, z, w
    }
#else
        : _xyzw(_mm_set_ps(x, y, z, w))
#endif
    {
    }

    DRAKO_FORCE_INLINE constexpr vec4::vec4(const std::array<float, 4>& xyzw) noexcept
#if !defined(DRAKO_API_SIMD)
        : _xyzw
    {
        xyzw[0], xyzw[1], xyzw[2], xyzw[3]
    }
#else
        : _xyzw(_mm_set_ps(xyzw[0], xyzw[1], xyzw[2], xyzw[3]))
#endif
    {
    }

    // Increments the vector.
    DRAKO_FORCE_INLINE constexpr vec4& vec4::operator+=(const vec4& v) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xyzw[0] += v[0];
        _xyzw[1] += v[1];
        _xyzw[2] += v[2];
        _xyzw[3] += v[3];
#else
        _xyzw       = _mm_add_ps(_xyzw, v._xyzw);
#endif
        return *this;
    }

    // Decrements the vector.
    DRAKO_FORCE_INLINE constexpr vec4& vec4::operator-=(const vec4& v) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xyzw[0] -= v[0];
        _xyzw[1] -= v[1];
        _xyzw[2] -= v[2];
        _xyzw[3] -= v[3];
#else
        _xyzw       = _mm_sub_ps(_xyzw, v._xyzw);
#endif
        return *this;
    }

    // Component-wise multiplication with a scalar.
    DRAKO_FORCE_INLINE constexpr vec4& vec4::operator*=(float f) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xyzw[0] *= f;
        _xyzw[1] *= f;
        _xyzw[2] *= f;
        _xyzw[3] *= f;
#else
#error
#endif
        return *this;
    }

    // Scales the vector.
    DRAKO_FORCE_INLINE constexpr vec4& vec4::operator*=(const vec4& v) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xyzw[0] *= v[0];
        _xyzw[1] *= v[1];
        _xyzw[2] *= v[2];
        _xyzw[3] *= v[3];
#else
        _xyzw       = _mm_mul_ps(_xyzw, v._xyzw);
#endif
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr vec4& vec4::operator/=(const vec4& v) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _xyzw[0] /= v[0];
        _xyzw[1] /= v[1];
        _xyzw[2] /= v[2];
        _xyzw[3] /= v[3];
#else
        this->_xyzw = _mm_mul_ps(this->_xyzw, v._xyzw);
#endif
        return *this;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec4 operator+(const vec4& lhs, const vec4& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec4(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2], lhs[3] + rhs[3]);
#else
        return _mm_add_ps(lhs, rhs);
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec4 operator-(const vec4& lhs, const vec4& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec4(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2], lhs[3] - rhs[3]);
#else
        return _mm_sub_ps(lhs, rhs);
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec4 operator*(const vec4& lhs, const vec4& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec4(lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2], lhs[3] * rhs[3]);
#else
        return _mm_mul_ps(lhs, rhs);
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr vec4 operator/(const vec4& lhs, const vec4& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return vec4(lhs[0] / rhs[0], lhs[1] / rhs[1], lhs[2] / rhs[2], lhs[3] / rhs[3]);
#else
        return _mm_div_ps(lhs, rhs);
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float& vec4::operator[](size_t idx) noexcept
    {
#if !defined(DRAKO_SIMD)
        return _xyzw[idx];
#else
#error
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float vec4::operator[](size_t idx) const noexcept
    {
#if !defined(DRAKO_SIMD)
        return _xyzw[idx];
#else
#error
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr bool operator==(const vec4& lhs, const vec4& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return (lhs[0] == rhs[0]) & (lhs[1] == rhs[1]) & (lhs[2] == rhs[2]) & (lhs[3] == rhs[3]);
#else
        return;
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr bool operator!=(const vec4& lhs, const vec4& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    // Computes the dot product.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float dot(const vec4& lhs, const vec4& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3];
#else
        return _mm_dp_ps(v1, v2, 0b0001).m128_f32[0];
#endif
    }

    // Computes the lenght of the vector.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE float norm(const vec4& v) noexcept
    {
        return std::sqrt(dot(v, v));
    }

    // Computes the squared lenght of the vector.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float sqrd_norm(const vec4& v) noexcept
    {
        return dot(v, v);
    }
} // namespace drako

#endif // !DRAKO_VECTOR4_INL