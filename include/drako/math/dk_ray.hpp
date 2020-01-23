#pragma once
#ifndef DRAKO_RAY_HPP
#define DRAKO_RAY_HPP

#include "drako/core/preprocessor/compiler_macros.hpp"
#include "drako/math/dk_vector2.hpp"

namespace drako
{
    // Represents an infinite line starting from a point and moving in a direction.
    //
    struct Ray2D
    {
        // Origin of the ray.
        Point2D origin;

        // Direction of the ray.
        vec2 direction;
    };


    // Returns squared distance between point P and ray R.
    //
    DRAKO_NODISCARD constexpr float
        sqrd_distance(Point2D P, Ray2D R) noexcept
    {
        // PRECON: ray direction d is unit lenght

        const auto dPL = P - R.origin;
        const auto proj = dot(R.direction, dPL);

        if (proj > 0.0f) // point P is projected on the ray
        {
            return dot(dPL, dPL) - proj * proj;
        }
        else // point P falls outside the ray, closest point is origin
        {
            return dot(dPL, dPL);
        }
    }

} // namespace drako::math

#endif // !DRAKO_RAY_HPP
