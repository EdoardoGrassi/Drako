#pragma once
#ifndef DRAKO_AABOX_HPP
#define DRAKO_AABOX_HPP

#include <algorithm>
#include <limits>
#include <tuple>

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/math/line.hpp"
#include "drako/math/vector3.hpp"

namespace drako
{

    // Axis-aligned box with min-max vertices representation.
    struct _aabox_mix_max
    {
        vec3 min;
        vec3 max;

        // Constructs a box with the given origin and size.
        DRAKO_FORCE_INLINE constexpr explicit _aabox_mix_max(vec3 center, vec3 extents) noexcept;
    };

    using aabox = _aabox_mix_max;

    // Constructs a box from two vertexes.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr aabox
    aabox_from_corners(vec3 min, vec3 max) noexcept;

    // Constructs a box from a vertex and box size.
    DRAKO_NODISCARD DRAKO_FORCE_INLINE constexpr aabox
    aabox_from_vertex(vec3 min, vec3 extents) noexcept;


    // Test if point P is contained in aab B volume.
    DRAKO_NODISCARD constexpr bool
    overlap(const vec3& p, const aabox& b) noexcept
    {
    }

    // Closest point contained in aabox B to point P.
    DRAKO_NODISCARD constexpr vec3
    closest_point(const vec3& p, const aabox& b) noexcept
    {
        // clamp point coordinates to be inside B boundaries if P lies outside
        return max(min(p, b.max), b.min);
    }

    // Squared distance between point P and aabox B.
    DRAKO_NODISCARD constexpr float
    squared_distance(const vec3& p, const aabox& b) noexcept;


    DRAKO_NODISCARD constexpr std::tuple<bool, float, vec3>
    intersect(vec3 const p, vec3 const d, aabox const b) noexcept
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
                    return { false, std::numeric_limits<float>::infinity(), {} };
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
            return { true, tmin, p + d * tmin };
        }
        return { false, std::numeric_limits<float>::infinity(), {} };
    }

} // namespace drako

#endif // !DRAKO_AABOX_HPP