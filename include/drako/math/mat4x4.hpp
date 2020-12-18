#pragma once
#ifndef DRAKO_MAT4X4_HPP
#define DRAKO_MAT4X4_HPP

/// @file
///
/// @author Grassi Edoardo
/// @date   Last update: 03-09-2019

#include "drako/math/vector4.hpp"

#include <array>

namespace drako
{
    /// @brief Squared 4x4 matrix of single precision scalars.
    /// @note Column-major representation.
    class alignas(sizeof(float) * 16) CMMat4x4 final
    {
    public:
        explicit constexpr CMMat4x4() noexcept = default;

        explicit constexpr CMMat4x4(float f) noexcept
            : mm{ { f, f, f, f }, { f, f, f, f }, { f, f, f, f }, { f, f, f, f } }
        {
        }

        explicit constexpr CMMat4x4(float, float, float, float,
            float, float, float, float,
            float, float, float, float,
            float, float, float, float) noexcept;

        explicit constexpr CMMat4x4(const std::array<float, 16>& f) noexcept
            : mm{ f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7],
                f[8], f[9], f[10], f[11], f[12], f[13], f[14], f[15] }
        {
        }

        explicit constexpr CMMat4x4(const Vec4&, const Vec4&, const Vec4&, const Vec4&) noexcept;

        explicit constexpr CMMat4x4(
            const std::array<float, 4>& x,
            const std::array<float, 4>& y,
            const std::array<float, 4>& z,
            const std::array<float, 4>& w) noexcept
            : mm{ { x[0], x[1], x[2], x[3] }, { y[0], y[1], y[2], y[3] },
                { z[0], z[1], z[2], z[3] }, { w[0], w[1], w[2], w[3] } }
        {
        }

        constexpr CMMat4x4(const CMMat4x4&) noexcept = default;
        constexpr CMMat4x4& operator=(const CMMat4x4&) noexcept = default;


        constexpr CMMat4x4& operator+=(float f) noexcept
        {
            for (auto i = 0; i < 4; ++i)
                for (auto j = 0; j < 4; ++j)
                    mm[i][j] += f;
            return *this;
        }

        constexpr CMMat4x4& operator-=(float f) noexcept
        {
            for (auto i = 0; i < 4; ++i)
                for (auto j = 0; j < 4; ++j)
                    mm[i][j] -= f;
            return *this;
        }

        constexpr CMMat4x4& operator*=(float f) noexcept
        {
            for (auto i = 0; i < 4; ++i)
                for (auto j = 0; j < 4; ++j)
                    mm[i][j] *= f;
            return *this;
        }

        constexpr CMMat4x4& operator/=(float f) noexcept
        {
            for (auto i = 0; i < 4; ++i)
                for (auto j = 0; j < 4; ++j)
                    mm[i][j] /= f;
            return *this;
        }

        constexpr CMMat4x4& operator+=(const CMMat4x4& other) noexcept
        {
            for (auto i = 0; i < 4; ++i)
                for (auto j = 0; j < 4; ++j)
                    mm[i][j] += other[i][j];
            return *this;
        }

        constexpr CMMat4x4& operator-=(const CMMat4x4& other) noexcept
        {
            for (auto i = 0; i < 4; ++i)
                for (auto j = 0; j < 4; ++j)
                    mm[i][j] -= other[i][j];
            return *this;
        }

        [[nodiscard]] friend constexpr bool operator==(const CMMat4x4&, const CMMat4x4&) noexcept = default;
        [[nodiscard]] friend constexpr bool operator!=(const CMMat4x4&, const CMMat4x4&) noexcept = default;

        [[nodiscard]] constexpr float*       operator[](std::size_t pos) noexcept { return mm[pos]; }
        [[nodiscard]] constexpr const float* operator[](std::size_t pos) const noexcept { return mm[pos]; }

        /// @brief Computes the determinant of the matrix.
        inline constexpr friend float determinant(const CMMat4x4&) noexcept;

        /// @brief Computes the transposed of the matrix.
        constexpr friend CMMat4x4 transposed(const CMMat4x4& m) noexcept;

        /// @brief Computes Hadamard product (element-wise product).
        constexpr friend CMMat4x4 hadamard(const CMMat4x4&, const CMMat4x4&) noexcept;

        /// @brief Identity matrix.
        [[nodiscard]] static constexpr CMMat4x4 identity()
        {
            return CMMat4x4{ { 1.f, 0.f, 0.f, 0.f,
                0.f, 1.f, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                0.f, 0.f, 0.f, 1.f } };
        }

        float mm[4][4] = {};
    };
    static_assert(sizeof(CMMat4x4) == sizeof(float) * 16);
    static_assert(alignof(CMMat4x4) == sizeof(float) * 16);
    static_assert(std::is_standard_layout_v<CMMat4x4>);
    static_assert(std::is_trivially_copyable_v<CMMat4x4>);
    static_assert(std::is_nothrow_swappable_v<CMMat4x4>);
    static_assert(std::is_nothrow_destructible_v<CMMat4x4>);


    [[nodiscard]] constexpr CMMat4x4
    operator+(const CMMat4x4& lhs, const CMMat4x4& rhs) noexcept
    {
        CMMat4x4 ret;
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                ret[i][j] = lhs[i][j] + rhs[i][j];
        return ret;
    }

    [[nodiscard]] constexpr CMMat4x4
    operator-(const CMMat4x4& lhs, const CMMat4x4& rhs) noexcept
    {
        CMMat4x4 ret;
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                ret[i][j] = lhs[i][j] - rhs[i][j];
        return ret;
    }

    [[nodiscard]] constexpr CMMat4x4
    operator*(const CMMat4x4& lhs, const CMMat4x4& rhs) noexcept
    {
        CMMat4x4 mul;
        for (auto r = 0; r < 4; ++r)
            for (auto c = 0; c < 4; ++c)
                mul[r][c] = lhs[r][0] * rhs[0][c] + lhs[r][1] * rhs[1][c] +
                            lhs[r][2] * rhs[2][c] + lhs[r][3] * rhs[3][c];
        return mul;
    }

    /*
    [[nodiscard]] constexpr bool
    operator==(const CMMat4x4& lhs, const CMMat4x4& rhs) noexcept
    {
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                if (lhs[i][j] != rhs[i][j])
                    return false;
        return true;
    }

    [[nodiscard]] constexpr bool
    operator!=(const CMMat4x4& lhs, const CMMat4x4& rhs) noexcept
    {
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                if (lhs[i][j] == rhs[i][j])
                    return false;
        return true;
    }
    */


    [[nodiscard]] inline constexpr float determinant(const CMMat4x4& m) noexcept
    {
        // computes determinant of 3x3 matrix with Sarrus rule.
#define DRAKO_sarrus_rule(expr, r0, r1, r2, c0, c1, c2) \
    (expr(r0, c0) * expr(r1, c1) * expr(r2, c2)) -      \
        (expr(r0, c2) * expr(r1, c1) * expr(r2, c0)) +  \
        (expr(r0, c1) * expr(r1, c2) * expr(r2, c0)) -  \
        (expr(r0, c0) * expr(r1, c2) * expr(r2, c1)) +  \
        (expr(r0, c2) * expr(r1, c0) * expr(r2, c1)) -  \
        (expr(r0, c1) * expr(r1, c0) * expr(r2, c2))

        // expression parameter used in DRAKO_sarrus_rule macro.
#define DRAKO_sarrus_expr(r, c) m[r][c]

        const auto d00 = DRAKO_sarrus_rule(DRAKO_sarrus_expr, 1, 2, 3, 1, 2, 3);
        const auto d01 = DRAKO_sarrus_rule(DRAKO_sarrus_expr, 1, 2, 3, 0, 2, 3);
        const auto d02 = DRAKO_sarrus_rule(DRAKO_sarrus_expr, 1, 2, 3, 0, 1, 3);
        const auto d03 = DRAKO_sarrus_rule(DRAKO_sarrus_expr, 1, 2, 3, 0, 1, 2);

#undef DRAKO_sarrus_expr
#undef DRAKO_sarrus_rule

        return (d00 * m[0][0]) - (d01 * m[0][1]) + (d02 * m[0][1]) - (d03 * m[0][1]);
    }

    [[nodiscard]] constexpr CMMat4x4 transposed(const CMMat4x4& m) noexcept
    {
        CMMat4x4 ret{};
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                ret[j][i] = m[i][j];
        return ret;
    }


    /// @brief Computes the trace of the matrix (sum of the elements on the main diagonal).
    ///
    [[nodiscard]] constexpr float trace(const CMMat4x4& m) noexcept
    {
        return m[0][0] + m[1][1] + m[2][2] + m[3][3];
    }

    [[nodiscard]] constexpr CMMat4x4
    hadamard(const CMMat4x4& lhs, const CMMat4x4& rhs) noexcept
    {
        CMMat4x4 ret{};
        for (auto i = 0; i < 4; ++i)
            for (auto j = 0; j < 4; ++j)
                ret[i][j] = lhs[i][j] * rhs[i][j];
        return ret;
    }

    /// @brief Computes the inverse of the matrix.
    ///
    [[nodiscard]] constexpr CMMat4x4 inverse(const CMMat4x4& m) noexcept;
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

    using Mat4x4 = CMMat4x4;

} // namespace drako

#endif // !DRAKO_MAT4X4_HPP