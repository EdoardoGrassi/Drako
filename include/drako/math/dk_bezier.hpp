#pragma once
#ifndef DRAKO_BEZIER_HPP
#define DRAKO_BEZIER_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/math/dk_vector2.hpp"

namespace drako
{
    // CLASS
    // Quadratic bezier curve in 2D space.
    class QuadBezier2D
    {

    };

    // Samples the given quadratic bezier curve.
    //
    DRAKO_NODISCARD constexpr Point2D
        quadratic_bezier_position(Point2D p0, Point2D p1, Point2D p2, float t) noexcept
    {
        // B(t) = P1 + (1 - t)^2 * (P0 - P1) + t^2 * (P2 - P1)
        return p1 + (1 - t) * (1 - t) * (p0 - p1) + t * t * (p2 - p1);
    }


    // Samples the first derivative of given quadratic bezier curve.
    //
    DRAKO_NODISCARD constexpr Point2D
        quadratic_bezier_velocity(Point2D p0, Point2D p1, Point2D p2, float t) noexcept
    {
        // B(t)' = 2(1 - t) * (P1 - P0) + 2t * (P2 - P1)
        return 2 * (1 - t) * (p1 - p0) + 2 * t * (p2 - p1);
    }


    // Samples the second derivative of given quadratic bezier curve.
    //
    DRAKO_NODISCARD constexpr Point2D
        quadratic_bezier_acceleration(Point2D p0, Point2D p1, Point2D p2, float t) noexcept
    {
        // B(t)" = 2(P2 - 2 * P1 + P0)
        return 2 * (p2 - 2 * p1 + p0);
    }


    // CLASS
    // Cubic bezier curve in 2D space.
    class CubicBezier2D
    {

    };


    // Samples the given cubic bezier curve.
    //
    DRAKO_NODISCARD constexpr Point2D
        cubic_bezier_position(Point2D p0, Point2D p1, Point2D p2, Point2D p3, float t) noexcept
    {
        // B(t) = (1 - t)^3 * P0 + 3t(1 - t)^2 * P1 + 3t^2 (1 - t) * P2 + t^3 * P3
        return (1 - t) * (1 - t) * (1 - t) * p0 +
            3 * t * (1 - t) * (1 - t) * p1 +
            3 * t * t * (1 - t) * p2 +
            t * t * t * p3;
    }

    // Samples the first derivative of given cubic bezier curve.
    //
    DRAKO_NODISCARD constexpr Point2D
        cubic_bezier_velocity(Point2D p0, Point2D p1, Point2D p2, Point2D p3, float t) noexcept
    {
        // B(t)' = 3(1 - t)^2 * (P1 - P0) + 6t(1 - t) * (P2 - P1) + 3t^2 * (P3 - P2)
        return 3 * (1 - t) * (1 - t) * (p1 - p0) +
            6 * t * (1 - t) * (p2 - p1) +
            3 * t * t * (p3 - p2);
    }

    // Samples the second derivative of given cubic bezier curve.
    //
    DRAKO_NODISCARD constexpr Point2D
        cubic_bezier_acceleration(Point2D p0, Point2D p1, Point2D p2, Point2D p3, float t) noexcept
    {
        // B(t)" = 6(1 - t)(P2 - 2 * P1 + P0) + 6t(P3 - 2 * P2 + P1)

        return 6 * (p2 - 2 * p1 + p0) + 6 * t * (p3 - 2 * p2 + p1);
    }


    // CLASS
    // 
    class BezierSpline2D
    {
        Point2D* _points;
        size_t      _size;
    };

} // namespace drako

#endif // !DRAKO_BEZIER_HPP
