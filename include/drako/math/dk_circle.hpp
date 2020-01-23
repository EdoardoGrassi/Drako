#pragma once
#ifndef DRAKO_CIRCLE_HPP
#define DRAKO_CIRCLE_HPP

#include "math/dk_vector2.hpp"

namespace drako::math
{
    // STRUCT
    // Circle in 2D space.
    struct alignas(sizeof(float) * 4) Circle
    {
        vec2 center;
        float radious;
    };

    // STRUCT
    // Capsule in 2D space.
    struct Capsule2D
    {
        vec2 c1;
        vec2 c2;
        float radious;
    };


} // namespace drako::math

#endif // !DRAKO_CIRCLE_HPP