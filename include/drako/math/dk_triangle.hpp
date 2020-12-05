#pragma once
#ifndef DRAKO_TRIANGLE_HPP
#define DRAKO_TRIANGLE_HPP

#include "core/compiler.hpp"
#include "math/dk_vector2.hpp"

namespace drako::math
{
    // STRUCT
    // Triangle shape with counterclockwise ordered vertices.
    struct Triangle2D
    {
        Point2D p[3];
    };


    // Returns squared distance between point P and closest point on triangle T.
    DRAKO_NODISCARD constexpr float
        sqrd_distance(Point2D P, Triangle2D T) noexcept
    {

    }


    // Tests if points A,B,C constitute a counterclockwise ordered triangle.
    DRAKO_NODISCARD constexpr bool
        is_ccw_triangle(Point2D a, Point2D b, Point2D c) noexcept
    {

    }

    // Test if 2D point is inside the generic 2D triangle ABC.
    DRAKO_NODISCARD constexpr bool
        overlap_triangle(Point2D p, Point2D a, Point2D b, Point2D c) noexcept
    {
        // P must be on the same side of every triangle edge
        auto pab = cross(p - a, b - a);
        auto pbc = cross(p - b, c - b);
        if (!same_sign(pab, pbc)) { return false; }

        auto pca = cross(p - c, a - c);
        return same_sign(pab, pca);
    }

    // Tests if 2D point P is inside the counterclockwise 2D triangle ABC.
    DRAKO_NODISCARD constexpr bool
        overlap_ccw_triangle(Point2D p, Point2D a, Point2D b, Point2D c) noexcept
    {
        // if P is on the right of AB then its outside triangle
        if (cross(p - a, b - a) > 0.0f) { return false; }
        // if P is on the right of BC then its outside triangle
        if (cross(p - b, c - b) > 0.0f) { return false; }
        // if P is on the right of CA then its outside triangle
        if (cross(p - c, a - c) > 0.0f) { return false; }

        return true;
    }

} // namespace drako::math

#endif // !DRAKO_TRIANGLE_HPP

