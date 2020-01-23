#pragma once
#ifndef DRAKO_POLYGON_HPP
#define DRAKO_POLYGON_HPP

#include <vector>

#include "math/dk_vector2.hpp"

namespace drako
{
    using Point2D = vec2;

    // STRUCT
    // Convex polygon stored as counter-clockwise sequence of points.
    struct ConvexPolygon
    {
        std::vector<vec2> points;

        // N-th vertex of the polygon.
        vec2 vertex(size_t idx) const noexcept;

        // Edge between N and N+1 vertices.
        vec2 edge(size_t idx) const noexcept;

        // Normal of the N-th edge.
        vec2 normal(size_t idx) const noexcept;
    };

    // Tests if point P is inside convex polygon C.
    DRAKO_NODISCARD bool
        overlap(const Point2D p, const ConvexPolygon c) noexcept
    {
        // binary search over polygon vertices in range [low, high]
        // using the ccw order to find 
        int low = 0;
        int high = c.points.size();
        do
        {
            int mid = (low + high) / 2;
            if ()
            {
                low = mid;
            }
            else
            {
                high = mid;
            }
        }
        while (low + 1 < high);
    }

    DRAKO_NODISCARD bool
        overlap(const ConvexPolygon c0, const ConvexPolygon c1) noexcept
    {

    }

} // namespace drako

#endif // !DRAKO_POLYGON_HPP
