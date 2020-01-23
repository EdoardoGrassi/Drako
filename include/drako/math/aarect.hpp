#pragma once
#ifndef DRAKO_AARECT_HPP
#    define DRAKO_AARECT_HPP

#    include "drako/core/preprocessor/compiler_macros.hpp"
#    include "drako/math/vector2.hpp"

namespace drako
{

    // Axis-aligned rectangle in 2D space.
    //
    struct AARect
    {
        vec2 min;
        vec2 max;

        DRAKO_FORCE_INLINE constexpr explicit AARect(vec2 min, vec2 max) noexcept
            : min(min), max(max)
        {
        }
    };

    // Test if point P overlaps rect R.
    DRAKO_NODISCARD constexpr bool overlap(const AARect& r, vec2 p) noexcept
    {
        return (p[0] >= r.min[0]) & (p[1] >= r.min[1]) & (p[0] <= r.max[0]) & (p[1] <= r.max[1]);
    }

    // Returns the point of rect R that is closest to given point P.
    DRAKO_NODISCARD constexpr vec2 closest_point(const AARect& r, vec2 p) noexcept
    {
        return max(min(r.max, p), r.min);
    }

    // Returns the point on rect R boundary that is closest to given point P.
    DRAKO_NODISCARD constexpr vec2 closest_boundary_point(const AARect& r, vec2 p) noexcept
    {
        // TODO: impl
        return vec2{};
    }

    // Returns the distance of point P from rect R.
    DRAKO_NODISCARD constexpr float distance(const AARect& r, vec2 p) noexcept
    {
        // TODO: impl
        return 0;
    }

} // namespace drako

#endif // !DRAKO_AARECT_HPP
