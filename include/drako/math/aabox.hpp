#pragma once
#ifndef DRAKO_AABOX_HPP
#define DRAKO_AABOX_HPP

#include "drako/math/line.hpp"
#include "drako/math/vector3.hpp"

#include <algorithm>
#include <limits>

namespace drako
{

    // Axis-aligned box with min-max vertices representation.
    struct _aabox_mix_max
    {
        Vec3 min;
        Vec3 max;

        // Constructs a box with the given origin and size.
        constexpr explicit _aabox_mix_max(Vec3 center, Vec3 extents) noexcept;
    };

    using AABox = _aabox_mix_max;

    // Constructs a box from two vertexes.
    [[nodiscard]] constexpr AABox
    aabox_from_corners(Vec3 min, Vec3 max) noexcept;

    // Constructs a box from a vertex and box size.
    [[nodiscard]] constexpr AABox
    aabox_from_vertex(Vec3 min, Vec3 extents) noexcept;


    // Test if point P is contained in aab B volume.
    [[nodiscard]] constexpr bool overlap(const Vec3& p, const AABox& b) noexcept;

    // Closest point contained in AABox B to point P.
    [[nodiscard]] constexpr Vec3 closest_point(const Vec3& p, const AABox& b) noexcept
    {
        // clamp point coordinates to be inside B boundaries if P lies outside
        return max(min(p, b.max), b.min);
    }

    // Squared distance between point P and AABox B.
    [[nodiscard]] constexpr float squared_distance(const Vec3& p, const AABox& b) noexcept;


    struct IntersectResult
    {
        Vec3  where;
        float distance;
        bool  found;
    };
    [[nodiscard]] constexpr IntersectResult intersect(Vec3 const p, Vec3 const d, AABox const b) noexcept
    {
        float tmin = 0.0f;
        float tmax = std::numeric_limits<float>::max();

        // For all three slabs that compose the box
        for (auto i = 0; i < 3; i++)
        {
            if (std::abs(d[i]) < std::numeric_limits<float>::epsilon())
            {
                // Ray is parallel to slab. No hit if origin is within slab
                if (p[i] < b.min[i] || p[i] > b.max[i])
                    return { Vec3{}, std::numeric_limits<float>::infinity(), false };
            }
            else
            {
                // Compute intersection t value of ray with near and far plane of slab
                auto t1 = (b.min[i] - p[i]) / d[i];
                auto t2 = (b.max[i] - p[i]) / d[i];

                if (t1 > t2)
                {
                    std::swap(t1, t2);
                }

                // Compute intersections of slabs intersection intervals
                tmin = std::fmaxf(tmin, t1);
                tmax = std::fmaxf(tmax, t2);
            }
            // Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
            return { p + d * tmin, tmin, true };
        }
        return { Vec3{}, std::numeric_limits<float>::infinity(), false };
    }

} // namespace drako

#endif // !DRAKO_AABOX_HPP