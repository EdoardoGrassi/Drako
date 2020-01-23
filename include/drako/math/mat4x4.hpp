#pragma once
#ifndef DRAKO_MAT4X4_HPP
#define DRAKO_MAT4X4_HPP

/// @file
///
/// @author Grassi Edoardo
/// @date   Last update: 03-09-2019

#include <array>
#include <cstdlib>

#if defined(DRAKO_API_SIMD)
#include <immintrin.h>
#endif

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/devel/assertion.hpp"
#include "drako/math/vector4.hpp"

namespace drako
{
    /// @brief Squared 4x4 matrix of single precision scalars. Column-major representation.
    class alignas(sizeof(float) * 16) _cm_mat4x4 final
    {
    public:
        using _cmm4 = _cm_mat4x4;

        static constexpr size_t _col_count = 4;
        static constexpr size_t _row_count = 4;

        DRAKO_FORCE_INLINE explicit constexpr _cm_mat4x4() noexcept;
        DRAKO_FORCE_INLINE explicit constexpr _cm_mat4x4(float) noexcept;
        DRAKO_FORCE_INLINE explicit constexpr _cm_mat4x4(float, float, float, float,
            float, float, float, float,
            float, float, float, float,
            float, float, float, float) noexcept;
        DRAKO_FORCE_INLINE explicit constexpr _cm_mat4x4(const std::array<float, 16>&) noexcept;
        DRAKO_FORCE_INLINE explicit constexpr _cm_mat4x4(const vec4&, const vec4&, const vec4&, const vec4&) noexcept;
        DRAKO_FORCE_INLINE explicit constexpr _cm_mat4x4(
            const std::array<float, 4>&,
            const std::array<float, 4>&,
            const std::array<float, 4>&,
            const std::array<float, 4>&) noexcept;

        constexpr _cm_mat4x4(const _cmm4&) noexcept = default;
        constexpr _cmm4& operator=(const _cmm4&) noexcept = default;

        DRAKO_FORCE_INLINE constexpr _cmm4& operator+=(float) noexcept;
        DRAKO_FORCE_INLINE constexpr _cmm4& operator-=(float) noexcept;
        DRAKO_FORCE_INLINE constexpr _cmm4& operator*=(float) noexcept;
        DRAKO_FORCE_INLINE constexpr _cmm4& operator/=(float) noexcept;
        DRAKO_FORCE_INLINE constexpr _cmm4& operator+=(const _cmm4&) noexcept;
        DRAKO_FORCE_INLINE constexpr _cmm4& operator-=(const _cmm4&) noexcept;

        DRAKO_FORCE_INLINE constexpr friend _cmm4 operator+(const _cmm4&, float) noexcept;
        DRAKO_FORCE_INLINE constexpr friend _cmm4 operator+(float, const _cmm4&) noexcept;
        DRAKO_FORCE_INLINE constexpr friend _cmm4 operator+(const _cmm4&, const _cmm4&) noexcept;

        DRAKO_FORCE_INLINE constexpr friend _cmm4 operator-(const _cmm4&, float) noexcept;
        DRAKO_FORCE_INLINE constexpr friend _cmm4 operator-(float, const _cmm4&) noexcept;
        DRAKO_FORCE_INLINE constexpr friend _cmm4 operator-(const _cmm4&, const _cmm4&) noexcept;

        DRAKO_FORCE_INLINE constexpr friend _cmm4 operator*(const _cmm4&, float)noexcept;
        DRAKO_FORCE_INLINE constexpr friend _cmm4 operator*(float, const _cmm4&)noexcept;

        DRAKO_FORCE_INLINE constexpr friend _cmm4 operator/(const _cmm4&, float) noexcept;
        DRAKO_FORCE_INLINE constexpr friend _cmm4 operator/(float, const _cmm4&) noexcept;

        DRAKO_FORCE_INLINE constexpr friend _cmm4 operator*(const _cmm4&, const _cmm4&)noexcept;

        DRAKO_FORCE_INLINE constexpr friend bool operator==(const _cmm4&, const _cmm4&) noexcept;
        DRAKO_FORCE_INLINE constexpr friend bool operator!=(const _cmm4&, const _cmm4&) noexcept;

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float* operator[](size_t idx) noexcept;
        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr const float* operator[](size_t idx) const noexcept;

        /// @brief Computes the determinant of the matrix.
        inline constexpr friend float determinant(const _cm_mat4x4&) noexcept;

        /// @brief Computes the transposed of the matrix.
        DRAKO_FORCE_INLINE constexpr friend _cm_mat4x4 transposed(const _cm_mat4x4& m) noexcept;

        /// @brief Computes Hadamard product (element-wise product).
        DRAKO_FORCE_INLINE constexpr friend _cm_mat4x4 hadamard(const _cm_mat4x4&, const _cm_mat4x4&) noexcept;

        // Identity matrix.
        DRAKO_NODISCARD DRAKO_FORCE_INLINE static constexpr _cm_mat4x4 identity()
        {
            return _cm_mat4x4{{1.f, 0.f, 0.f, 0.f,
                0.f, 1.f, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                0.f, 0.f, 0.f, 1.f}};
        }

    private:
#if !defined(DRAKO_SIMD)
        float _mm[_col_count][_row_count];
#else
        __m512 _mm;

        DRAKO_FORCE_INLINE constexpr _cm_mat4x4(const __m512& values) noexcept
            : _mm(values)
        {
        }

        DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr operator __m512() const noexcept { return _mm; }

#endif
    };
    static_assert(sizeof(_cm_mat4x4) == sizeof(float) * 16);
    static_assert(alignof(_cm_mat4x4) == sizeof(float) * 16);
    static_assert(std::is_standard_layout_v<_cm_mat4x4>);
    static_assert(std::is_trivially_copyable_v<_cm_mat4x4>);
    static_assert(std::is_nothrow_swappable_v<_cm_mat4x4>);
    static_assert(std::is_nothrow_destructible_v<_cm_mat4x4>);

    // using _cm_mat4x4 = _column_major_mat4x4;


    DRAKO_FORCE_INLINE constexpr _cm_mat4x4::_cm_mat4x4(float f) noexcept
#if !defined(DRAKO_API_SIMD)
        : _mm{{f, f, f, f}, {f, f, f, f}, {f, f, f, f}, {f, f, f, f}}
    {
    }
#else
        : _mm(_mm512_set_ps1(f))
    {
    }
#endif

    DRAKO_FORCE_INLINE constexpr _cm_mat4x4::_cm_mat4x4(const std::array<float, 16>& f) noexcept
#if !defined(DRAKO_API_SIMD)
        : _mm{f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7],
              f[8], f[9], f[10], f[11], f[12], f[13], f[14], f[15]}
    {
    }
#else
        : _mm(_mm512_set_ps(f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7],
              f[8], f[9], f[10], f[11], f[12], f[13], f[14], f[15]))
    {
    }
#endif

    DRAKO_FORCE_INLINE constexpr _cm_mat4x4::_cm_mat4x4(
        const std::array<float, 4>& x,
        const std::array<float, 4>& y,
        const std::array<float, 4>& z,
        const std::array<float, 4>& w) noexcept
#if !defined(DRAKO_API_SIMD)
        : _mm{{x[0], x[1], x[2], x[3]}, {y[0], y[1], y[2], y[3]},
              {z[0], z[1], z[2], z[3]}, {w[0], w[1], w[2], w[3]}}
    {
    }
#else
#error
#endif

    DRAKO_FORCE_INLINE constexpr _cm_mat4x4& _cm_mat4x4::operator+=(const _cm_mat4x4& m) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                _mm[i][j] += m[i][j];
#else
        _mm = _mm512_add_ps(_mm, m._mm);
#endif
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr _cm_mat4x4& _cm_mat4x4::operator-=(const _cm_mat4x4& m) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                _mm[i][j] -= m[i][j];
#else
        _mm = _mm512_sub_ps(_mm, m._mm);
#endif
        return *this;
    }

    DRAKO_FORCE_INLINE constexpr _cm_mat4x4& _cm_mat4x4::operator*=(float f) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                _mm[i][j] *= f;
#else
        _mm = _mm512_mul_ps(_mm, _mm_set1_ps(f));
#endif
        return *this;
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr _cm_mat4x4
    operator+(const _cm_mat4x4& lhs, const _cm_mat4x4& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _cm_mat4x4 ret;
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                ret[i][j] = lhs[i][j] + rhs[i][j];
        return ret;
#else
        return _mm512_add_ps(lhs, rhs);
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr _cm_mat4x4
    operator-(const _cm_mat4x4& lhs, const _cm_mat4x4& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _cm_mat4x4 ret;
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                ret[i][j] = lhs[i][j] - rhs[i][j];
        return ret;
#else
        return _mm512_sub_ps(lhs, rhs);
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr bool
    operator==(const _cm_mat4x4& lhs, const _cm_mat4x4& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                if (lhs[i][j] != rhs[i][j])
                    return false;
        return true;
#else
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr bool
    operator!=(const _cm_mat4x4& lhs, const _cm_mat4x4& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                if (lhs[i][j] == rhs[i][j])
                    return false;
        return true;
#else
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr _cm_mat4x4
    operator*(const _cm_mat4x4& lhs, const _cm_mat4x4& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _cm_mat4x4 mul;
        for (auto r = 0; r < 4; ++r)
            for (auto c = 0; c < 4; ++c)
                mul[r][c] = lhs[r][0] * rhs[0][c] +
                            lhs[r][1] * rhs[1][c] +
                            lhs[r][2] * rhs[2][c] +
                            lhs[r][3] * rhs[3][c];
        return mul;
#else
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float*
        _cm_mat4x4::operator[](size_t idx) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return _mm[idx];
#else
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr const float*
        _cm_mat4x4::operator[](size_t idx) const noexcept
    {
#if !defined(DRAKO_API_SIMD)
        return _mm[idx];
#else
#endif
    }

    DRAKO_NODISCARD inline constexpr float determinant(const _cm_mat4x4& m) noexcept
    {
#if !defined(DRAKO_API_SIMD)

// MACRO: computes determinant of 3x3 matrix with Sarrus rule.
#define DRAKO_sarrus_rule(expr, r0, r1, r2, c0, c1, c2) \
    (expr(r0, c0) * expr(r1, c1) * expr(r2, c2)) -      \
        (expr(r0, c2) * expr(r1, c1) * expr(r2, c0)) +  \
        (expr(r0, c1) * expr(r1, c2) * expr(r2, c0)) -  \
        (expr(r0, c0) * expr(r1, c2) * expr(r2, c1)) +  \
        (expr(r0, c2) * expr(r1, c0) * expr(r2, c1)) -  \
        (expr(r0, c1) * expr(r1, c0) * expr(r2, c2))

        // MACRO: expression parameter used in DRAKO_sarrus_rule macro.
#define DRAKO_sarrus_expr(r, c) m[r][c]

        const auto d00 = DRAKO_sarrus_rule(DRAKO_sarrus_expr, 1, 2, 3, 1, 2, 3);
        const auto d01 = DRAKO_sarrus_rule(DRAKO_sarrus_expr, 1, 2, 3, 0, 2, 3);
        const auto d02 = DRAKO_sarrus_rule(DRAKO_sarrus_expr, 1, 2, 3, 0, 1, 3);
        const auto d03 = DRAKO_sarrus_rule(DRAKO_sarrus_expr, 1, 2, 3, 0, 1, 2);

#undef DRAKO_sarrus_expr
#undef DRAKO_sarrus_rule

        return (d00 * m[0][0]) - (d01 * m[0][1]) + (d02 * m[0][1]) - (d03 * m[0][1]);
#else
#error Not implemented
#endif
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr _cm_mat4x4
    transposed(const _cm_mat4x4& m) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _cm_mat4x4 ret{};
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                ret[j][i] = m[i][j];
        return ret;
#else
#error Not implemented
#endif
    }


    /// @brief Computes the trace of the matrix (sum of the elements on the main diagonal).
    ///
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float
    trace(const _cm_mat4x4& m) noexcept
    {
        return m[0][0] + m[1][1] + m[2][2] + m[3][3];
    }

    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr _cm_mat4x4
    hadamard(const _cm_mat4x4& lhs, const _cm_mat4x4& rhs) noexcept
    {
#if !defined(DRAKO_API_SIMD)
        _cm_mat4x4 ret{};
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                ret[i][j] = lhs[i][j] * rhs[i][j];
        return ret;
#else
        return _mm512_mul_ps(lhs, rhs);
#endif
    }

    /// @brief Computes the inverse of the matrix.
    ///
    DRAKO_NODISCARD constexpr _cm_mat4x4 inverse(const _cm_mat4x4& m) noexcept;
    /*{
        // TODO: end impl
        // matrix inversion with Cayley-Hamilton method

        const auto det = m.determinant();
        DRAKO_ASSERT(det != 0.0f);

        const auto m2 = m * m;
        const auto m3 = m2 * m;
        const auto tm = trace(m);
        const auto t2 = trace(m2);
        const auto t3 = trace(m3);

        return 1.0f / det * (1.0f / 6.0f * (trc * trc * trc))
        return mat4x4::identity();
    }*/

    using mat4x4 = _cm_mat4x4;

} // namespace drako

#endif // !DRAKO_MAT4X4_HPP