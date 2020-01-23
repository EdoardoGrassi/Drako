#pragma once
#ifndef DRAKO_MATH_LINE_HPP
#define DRAKO_MATH_LINE_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/math/vector2.hpp"
#include "drako/math/vector3.hpp"

namespace drako
{
    // Parametrized line in 2D space.
    //
    struct line2d
    {
        vec2 origin;
        vec2 direction;
    };

    // Returns squared distance between point P and line L.
    DRAKO_NODISCARD constexpr float
    squared_distance(const vec2& p, const line2d& l) noexcept
    {
        DRAKO_ASSERT(std::abs(dot(l.direction, l.direction) - 1.f) < std::numeric_limits<float>::epsilon(),
            "Line direction must be normalized to allow formula reduction");
        // PRECON: line direction d is unit lenght

        const auto dPL  = p - l.origin;
        const auto proj = dot(l.direction, dPL);
        return dot(dPL, dPL) - proj * proj;
    }


    // Represents a line in 3d space.
    // Stored in parametric form L(t) = O + tN.
    struct line3d
    {
        vec3  origin;
        norm3 normal;
    };

    // Represents a segment in 3d space.
    // Stored in parametric form S(t) = A + tB.
    struct segment3d
    {
        vec3 a;
        vec3 b;
    };

    // Given point P, computes squared distance with nearest point on segment S.
    DRAKO_NODISCARD constexpr float
    squared_distance(const vec3& p, const segment3d& s) noexcept
    {
        const auto ab = s.b - s.a;
        const auto ap = p - s.a;
        const auto bp = p - s.b;

        const auto e = dot(ap, ab);
        if (e <= 0.f) // point P projects outside AB on A side
            return dot(ap, ap);

        const auto f = dot(ab, ab);
        if (e >= f) // point P projects outside AB on B side
            return dot(bp, bp);

        // point P projects onto AB
        return dot(ap, ap) - e * e / f;
    }

    // Given point P, computes closest point on segment S.
    DRAKO_NODISCARD constexpr vec3
    closest_point(const vec3& p, const segment3d& s) noexcept
    {
        const auto ab = s.b - s.a;
        const auto t  = dot(p - s.a, ab) / dot(ab, ab);
        // clamp t to the closest endpoint of the segment ab
        return s.a + clamp_zero_one(t) * ab;
    }

    // Returns 2 times the signed area of the triangle ABC.
    // The result is positive if ABC is ccw, negative if cw, zero if ABC degenerates.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr float
    signed_triangle_area(const vec3& a, const vec3& b, const vec3& c) noexcept
    {
        return (a[0] - c[0]) * (b[1] - c[1]) - (a[1] - c[1]) * (b[0] - c[0]);
    }

} // namespace drako

#endif // !DRAKO_MATH_LINE_HPP
