#pragma once
#ifndef DRAKO_LINE_HPP
#define DRAKO_LINE_HPP

#include "drako/core/compiler.hpp"
#include "drako/math/vector2.hpp"
#include "drako/math/vector3.hpp"

#include <cassert>

namespace drako
{
    // Parametrized line in 2D space.
    //
    struct line2d
    {
        Vec2 origin;
        Vec2 direction;
    };

    // Returns squared distance between point P and line L.
    [[nodiscard]] inline float squared_distance(const Vec2& p, const line2d& l) noexcept
    {
        assert(std::abs(dot(l.direction, l.direction) - 1.f) < std::numeric_limits<float>::epsilon());
        // "Line direction must be normalized to allow formula reduction"


        const auto dPL  = p - l.origin;
        const auto proj = dot(l.direction, dPL);
        return dot(dPL, dPL) - proj * proj;
    }


    // Represents a line in 3d space.
    // Stored in parametric form L(t) = O + tN.
    struct line3d
    {
        Vec3 origin;
        Vec3 normal;
    };

    // Represents a segment in 3d space.
    // Stored in parametric form S(t) = A + tB.
    struct segment3d
    {
        Vec3 a;
        Vec3 b;
    };

    // Given point P, computes squared distance with nearest point on segment S.
    [[nodiscard]] inline constexpr float
    squared_distance(const Vec3& p, const segment3d& s) noexcept
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
    [[nodiscard]] inline Vec3 closest_point(const Vec3& p, const segment3d& s) noexcept
    {
        const auto ab = s.b - s.a;
        const auto t  = dot(p - s.a, ab) / dot(ab, ab);
        // clamp t to the closest endpoint of the segment ab
        const auto clamped = std::fmax(std::fmin(t, 1.f), 0.f);
        return s.a + clamped * ab;
    }

    // Returns 2 times the signed area of the triangle ABC.
    // The result is positive if ABC is ccw, negative if cw, zero if ABC degenerates.
    [[nodiscard]] inline constexpr float
    signed_triangle_area(const Vec3& a, const Vec3& b, const Vec3& c) noexcept
    {
        return (a[0] - c[0]) * (b[1] - c[1]) - (a[1] - c[1]) * (b[0] - c[0]);
    }

} // namespace drako

#endif // !DRAKO_LINE_HPP
